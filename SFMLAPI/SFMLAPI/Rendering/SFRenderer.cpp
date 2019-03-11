#include "stdafx.h"
#include <mutex>
#include "Utils.h"
#include "Logger.h"
#include "SFRenderer.h"
#include "SFRenderState.h"
#include "SFMLAPI/Windowing/SFWindow.h"
#include "SFMLAPI/SFSystem.h"
#include "SFML/Graphics.hpp"

namespace LittleEngine
{
void RenderData::Reset()
{
	primitiveCount = staticCount = dynamicCount = framesPerSecond = 0;
	lastRenderTime = Time::Zero;
}

RenderData g_renderData;

#if ENABLED(RENDER_STATS)
namespace
{
Time fpsTime;
u32 frameCount = 0;
u32 fps = 0;

void UpdateFPS()
{
	++frameCount;
	g_renderData.lastRenderTime = Time::Now();
	if (Maths::Abs((fpsTime - g_renderData.lastRenderTime).AsSeconds()) >= 1.0f)
	{
		fpsTime = g_renderData.lastRenderTime;
		fps = frameCount;
		frameCount = 0;
	}
	g_renderData.framesPerSecond = fps;
}
} // namespace
#endif

SFRenderer::SFRenderer(SFWindow& sfWindow, Time tickRate)
	: m_pSFWindow(&sfWindow), m_tickRate(tickRate)
{
	m_bRendering.store(true, std::memory_order_relaxed);
}

void SFRenderer::Render(GFXBuffer& buffer)
{
	if (m_bRendering.load(std::memory_order_relaxed))
	{
		m_pSFWindow->clear();

		Fixed renderDT = Time::Now().AsMilliseconds() - buffer.GetLastSwapTime().AsMilliseconds();
		Fixed tickRate = m_tickRate.AsMilliseconds();
		Fixed alpha = Maths::Clamp01(renderDT / tickRate);
		buffer.Lock_Traverse([&](Vec<SFPrimitive>& active) {
#if ENABLED(RENDER_STATS)
			g_renderData.Reset();
#endif

			for (auto& primitive : active)
			{
				primitive.UpdateRenderState(alpha);
				m_pSFWindow->draw(primitive.m_circle);
				m_pSFWindow->draw(primitive.m_rectangle);
				m_pSFWindow->draw(primitive.m_sprite);
				m_pSFWindow->draw(primitive.m_text);

#if ENABLED(RENDER_STATS)
				++g_renderData.primitiveCount;
				if (primitive.m_bStatic)
					++g_renderData.staticCount;
#endif
			}

#if ENABLED(RENDER_STATS)
			g_renderData.dynamicCount = g_renderData.primitiveCount - g_renderData.staticCount;
			UpdateFPS();
#endif
		});
	}
}

void SFRenderer::Display()
{
	if (m_bRendering.load(std::memory_order_relaxed))
	{
		m_pSFWindow->display();
	}
}

void SFRenderer::SetWindowSize(const SFWindowSize& size)
{
	if (m_pSFWindow)
		m_pSFWindow->SetSize(size);
}
} // namespace LittleEngine
