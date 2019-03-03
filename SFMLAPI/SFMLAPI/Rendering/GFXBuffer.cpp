#include "stdafx.h"
#include "GFXBuffer.h"
#include "SFPrimitive.h"
#include "Utils.h"

namespace LittleEngine
{
GFXDataFrame::GFXDataFrame(Vec<SFPrimitive>&& primitives)
{
	for (const auto& primitive : primitives)
	{
		size_t idx = static_cast<size_t>(primitive.m_state.layer);
		Assert(layerArray.size() > idx, "Invalid layer on SFPrimitive!");
		auto& layer = layerArray.at(idx);
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

Vec<SFPrimitive> GFXDataFrame::CollapseAndMove()
{
	Vec<SFPrimitive> ret;
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

Vec<SFPrimitive>& GFXBuffer::ReferenceActiveBuffer()
{
	Vec<SFPrimitive>& source = m_bufferIdx == 1 ? m_buffer1 : m_buffer0;
	return source;
}

void GFXBuffer::Lock_Swap(GFXDataFrame&& newFrame, const Vector2& cullBounds)
{
	newFrame.Cull(cullBounds);
	Vec<SFPrimitive>* inactive = GetInactiveBuffer();
	*inactive = newFrame.CollapseAndMove();
	{
		Lock lock(m_bufferMutex);
		m_bufferIdx = 1 - m_bufferIdx;
		m_lastSwapTime = Time::Now();
	}
}

void GFXBuffer::Lock_Traverse(const std::function<void(Vec<SFPrimitive>& vec)>& Procedure)
{
	Lock lock(m_bufferMutex);
	Vec<SFPrimitive>& active = ReferenceActiveBuffer();
	Procedure(active);
}

Vec<SFPrimitive>* GFXBuffer::GetInactiveBuffer()
{
	return (m_bufferIdx == 1) ? &m_buffer0 : &m_buffer1;
}
} // namespace LittleEngine
