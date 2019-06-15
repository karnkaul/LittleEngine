#include "stdafx.h"
#include <mutex>
#include "SFML/Graphics.hpp"
#include "Core/Utils.h"
#include "Core/Logger.h"
#include "SFRenderer.h"
#include "ISFRenderBuffer.h"
#include "SFMLAPI/Rendering/SFShader.h"
#include "SFMLAPI/Viewport/SFViewport.h"
#include "SFMLAPI/Viewport/SFViewportData.h"

#include "SFMLAPI/Rendering/Primitives/Primitive.h"

namespace LittleEngine
{
#if ENABLED(RENDER_STATS)
RenderData g_renderData;
#endif

SFRenderer::SFRenderer(SFViewport& viewport) : m_pViewport(&viewport)
{
	m_bRendering.store(true);
}

SFRenderer::~SFRenderer() = default;

void SFRenderer::RenderFrame(IRenderBuffer& buffer, Fixed alpha)
{
	if (m_bRendering.load())
	{
		m_pViewport->clear();
		{
			std::lock_guard<std::mutex> lock(buffer.m_mutex);
			auto& matrix = buffer.GetActiveRenderMatrix();
#if ENABLED(RENDER_STATS)
			static Time fpsTime;
			static u32 frameCount = 0;
			u32 statics = 0;
			u32 drawCalls = 0;
			g_renderData._quadCount_Internal = 0;
#endif
			for (auto& vec : matrix)
			{
				for (auto& pPrimitive : vec)
				{
					if (pPrimitive->m_renderState.bEnabled)
					{
						pPrimitive->Draw(*m_pViewport, alpha);
						++drawCalls;
					}
#if ENABLED(RENDER_STATS)
					
					if (pPrimitive->IsStatic())
					{
						++statics;
					}
#endif
				}
			}
#if ENABLED(RENDER_STATS)
			g_renderData.quadCount = g_renderData._quadCount_Internal;
			g_renderData.staticCount = statics;
			g_renderData.drawCallCount = drawCalls;
			g_renderData.dynamicCount = drawCalls - statics;
			g_renderData.rendersPerFrame.fetch_add(1, std::memory_order_relaxed);
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
		}

		// Wait for VSync
		m_pViewport->display();
	}
}
} // namespace LittleEngine
