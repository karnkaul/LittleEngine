#include "stdafx.h"
#include <mutex>
#include "Utils.h"
#include "Logger.h"
#include "SFRenderer.h"
#include "SFRenderState.h"
#include "SFMLAPI/Windowing/SFWindow.h"
#include "SFMLAPI/Windowing/SFWindowData.h"
#include "SFML/Graphics.hpp"

namespace LittleEngine
{
#if ENABLED(RENDER_STATS)
RenderData g_renderData;
#endif

SFRenderer::SFRenderer(SFWindow& sfWindow, Time tickRate)
	: m_pSFWindow(&sfWindow), m_tickRate(tickRate)
{
	m_bRendering.store(true, std::memory_order_relaxed);
}

SFRenderer::~SFRenderer() = default;

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
			static Time fpsTime;
			static u32 frameCount = 0;
			u32 statics = 0;
			u32 primitives = 0;
#endif

			for (auto& primitive : active)
			{
				primitive.UpdateRenderState(alpha);
				m_pSFWindow->draw(primitive.m_circle);
				m_pSFWindow->draw(primitive.m_rectangle);
				m_pSFWindow->draw(primitive.m_sprite);
				m_pSFWindow->draw(primitive.m_text);

#if ENABLED(RENDER_STATS)
				++primitives;
				if (primitive.m_bStatic)
				{
					++statics;
				}
#endif
			}

#if ENABLED(RENDER_STATS)
			g_renderData.staticCount = statics;
			g_renderData.primitiveCount = primitives;
			g_renderData.dynamicCount = g_renderData.primitiveCount - g_renderData.staticCount;
			// Update FPS
			{
				++frameCount;
				g_renderData.lastRenderTime = Time::Now();
				if (Maths::Abs((fpsTime - g_renderData.lastRenderTime).AsSeconds()) >= 1.0f)
				{
					fpsTime = g_renderData.lastRenderTime;
					g_renderData.framesPerSecond = frameCount;
					frameCount = 0;
				}
			}
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
} // namespace LittleEngine
