#include "stdafx.h"
#include "GFXBuffer.h"
#include "SFPrimitive.h"
#include "Utils.h"

namespace LittleEngine
{
GFXDataFrame::GFXDataFrame(Vector<SFPrimitive>&& primitives)
{
	for (const auto& primitive : primitives)
	{
		RenderLayer& layer = layerArray[static_cast<size_t>(primitive.m_state.layer)];
		layer.emplace_back(SFPrimitive(primitive));
	}
}

bool GFXDataFrame::IsOutOfBounds(const Vector2& position, const Rect2& objectBounds, const Vector2& worldBounds)
{
	Vector2 tr = position + objectBounds.GetTopRight();
	Vector2 bl = position + objectBounds.GetBottomLeft();
	return (tr.x < -worldBounds.x || tr.y < -worldBounds.y || bl.x > worldBounds.x ||
			bl.y > worldBounds.y);
}

void GFXDataFrame::Cull(const Vector2& cullBounds)
{
	for (auto& layer : layerArray)
	{
		Core::CleanVector<SFPrimitive>(layer, [&](SFPrimitive& primitive) {
			return !primitive.m_state.bEnabled ||
				   IsOutOfBounds(primitive.m_state.sfPosition.min, primitive.GetBounds(), cullBounds);
		});
	}
}

Vector<SFPrimitive> GFXDataFrame::CollapseAndMove()
{
	Vector<SFPrimitive> ret;
	for (auto& layer : layerArray)
	{
		std::move(layer.begin(), layer.end(), std::back_inserter(ret));
	}
	return ret;
}

GFXBuffer::GFXBuffer()
{
	m_bufferIdx = 0;
}

Time GFXBuffer::GetLastSwapTime() const
{
	return m_lastSwapTime;
}

Vector<SFPrimitive>& GFXBuffer::ReferenceActiveBuffer()
{
	Vector<SFPrimitive>& source = m_bufferIdx == 1 ? m_buffer1 : m_buffer0;
	return source;
}

void GFXBuffer::Lock_Swap(GFXDataFrame&& newFrame, const Vector2& cullBounds)
{
	newFrame.Cull(cullBounds);
	Vector<SFPrimitive>* inactive = GetInactiveBuffer();
	*inactive = newFrame.CollapseAndMove();
	{
		Lock lock(m_bufferMutex);
		m_bufferIdx = 1 - m_bufferIdx;
		m_lastSwapTime = Time::Now();
	}
}

void GFXBuffer::Lock_Traverse(Function(void(Vector<SFPrimitive>& vec)) Procedure)
{
	Lock lock(m_bufferMutex);
	Vector<SFPrimitive>& active = ReferenceActiveBuffer();
	Procedure(active);
}

Vector<SFPrimitive>* GFXBuffer::GetInactiveBuffer()
{
	return (m_bufferIdx == 1) ? &m_buffer0 : &m_buffer1;
}
} // namespace LittleEngine
