#include "stdafx.h"
#include <mutex>
#include "Core/Utils.h"
#include "Core/Logger.h"
#include "SFPrimitive.h"
#include "SFRenderer.h"
#include "SFRenderState.h"
#include "ISFRenderBuffer.h"
#include "SFMLAPI/Windowing/SFWindow.h"
#include "SFMLAPI/Windowing/SFWindowData.h"
#include "SFML/Graphics.hpp"

namespace LittleEngine
{
#if ENABLED(RENDER_STATS)
RenderData g_renderData;
#endif

SFRenderer::SFRenderer(SFWindow& sfWindow)
	: m_pSFWindow(&sfWindow)
{
	m_bRendering.store(true, std::memory_order_relaxed);
}

SFRenderer::~SFRenderer() = default;

void SFRenderer::Render(IRenderBuffer& buffer, Fixed alpha)
{
	if (m_bRendering.load(std::memory_order_relaxed))
	{
		m_pSFWindow->clear();

		// Lock mutex
		buffer.m_mutex.lock();
		auto& matrix = buffer.GetActiveRenderMatrix();
#if ENABLED(RENDER_STATS)
		static Time fpsTime;
		static u32 frameCount = 0;
		u32 statics = 0;
		u32 disabled = 0;
		u32 primitives = 0;
		u32 quads = 0;
#endif
		for (auto& vec : matrix)
		{
			for (auto& pPrimitive : vec)
			{
				if (pPrimitive->m_quadVec.IsPopulated())
				{
					sf::VertexArray va = pPrimitive->m_quadVec.ToSFVertexArray();
					sf::RenderStates rs = pPrimitive->m_quadVec.ToSFRenderStates();
#if ENABLED(RENDER_STATS)
					quads += (va.getVertexCount() / 4);
#endif
					m_pSFWindow->draw(va, rs);
				}
				else
				{
					pPrimitive->UpdateRenderState(alpha);
					m_pSFWindow->draw(pPrimitive->m_circle);
					m_pSFWindow->draw(pPrimitive->m_rectangle);
					m_pSFWindow->draw(pPrimitive->m_sprite);
					m_pSFWindow->draw(pPrimitive->m_text);

#if ENABLED(RENDER_STATS)
					++primitives;
					if (!pPrimitive->m_renderState.bEnabled)
					{
						++disabled;
					}
					if (pPrimitive->m_bStatic)
					{
						++statics;
					}
#endif
				}
			}
		}
#if ENABLED(RENDER_STATS)
		g_renderData.staticCount = statics;
		g_renderData.disabledCount = disabled;
		g_renderData.primitiveCount = primitives;
		g_renderData.dynamicCount = primitives - statics - disabled;
		g_renderData.quadCount = quads;
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

		// Release lock
		buffer.m_mutex.unlock();
		
		// Wait for VSync
		m_pSFWindow->display();
	}
}
} // namespace LittleEngine
