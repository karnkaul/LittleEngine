#include "Core/Game/OS.h"
#include "SFMLAPI/Rendering/RenderStats.h"
#include "SFMLAPI/Rendering/Primitives/Quads.h"
#include "SFMLAPI/System/SFTypes.h"
#include "SFMLAPI/Viewport/ViewportData.h"
#include "SFMLAPI/Viewport/Viewport.h"
#include "LERenderer.h"
#include "Engine/Debug/Profiler.h"
#include "Engine/Debug/Tweakable.h"
#include "Engine/GFX.h"

namespace LittleEngine
{
LERenderer::Data::Data() = default;
LERenderer::Data::Data(Time tickRate, Time threadStartDelay, u32 maxFPS, bool bStartThread)
	: tickRate(tickRate), threadStartDelay(threadStartDelay), maxFPS(maxFPS), bStartThread(bStartThread)
{
}

LERenderer::LERenderer(Viewport& viewport, Data data) : m_data(data), m_pViewport(&viewport)
{
#if ENABLED(RENDER_STATS)
	g_renderStats.fpsMax = m_data.maxFPS;
#endif
	Start();
}

LERenderer::~LERenderer()
{
	if (m_bRendering.load(std::memory_order_relaxed))
	{
		Stop();
	}
}

bool LERenderer::IsRunningRenderThread() const
{
	return m_threadHandle > 0 && m_bRendering.load(std::memory_order_relaxed);
}

void LERenderer::RecreateViewport(ViewportRecreateData data)
{
	Assert(OS::IsMainThread(), "Renderer::RecreateWindow() not called from Main thread!");
	Stop();
	m_pViewport->setActive(true);
	m_pViewport->Destroy();
	m_pViewport->OverrideData(std::move(data));

	m_pViewport->Create(m_data.maxFPS);
	LOG_D("[Renderer] Activated viewport on this thread");
	Start();
}

void LERenderer::Lock_Swap()
{
	PROFILE_CUSTOM("SWAP", m_data.tickRate, Colour(255, 72, 0));
	Lock lock(m_swapMutex);
	Swap();
	PROFILE_STOP("SWAP");
}

void LERenderer::Lock_Reconcile() 
{
	PROFILE_CUSTOM("RECONCILE", m_data.tickRate, Colour(255, 72, 0));
	Lock lock(m_swapMutex);
	Reconcile();
	PROFILE_STOP("RECONCILE");
}

void LERenderer::Render(Fixed alpha)
{
	{
		Lock lock(m_swapMutex);
		RenderFrame(*this, alpha);
	}
	WaitForVsync();
}

void LERenderer::RenderFrame(PrimitiveFactory& buffer, Fixed alpha)
{
	m_pViewport->clear();
#if ENABLED(RENDER_STATS)
	static Time fpsTime;
	static u32 frameCount = 0;
	u32 statics = 0;
	u32 drawCalls = 0;
	g_renderStats._quadCount_Internal = 0;
#endif
	sf::Vector2 zero = Cast(Vector2::Zero);
	sf::View worldView(Cast(g_pGFX->LerpedWorldPosition(alpha)), Cast(g_pGFX->LerpedWorldSpace(alpha)));
	sf::View uiView(zero, Cast(g_pGFX->UISpace()));
	sf::View overlayView(zero, Cast(g_pGFX->OverlaySpace()));
	uiView.setViewport(g_pGFX->UIViewCrop());

	// Set World View
	m_pViewport->setView(worldView);

	auto& matrix = buffer.ActiveRenderMatrix();
	for (size_t layer = 0; layer < matrix.size(); ++layer)
	{
		switch (layer)
		{
		default:
			break;

		case ToIdx(LayerID::UnderlayFX):
		case ToIdx(LayerID::OverlayFX):
		case ToIdx(LayerID::Background):
#ifdef DEBUGGING
		case ToIdx(LayerID::TopOverlay):
#endif
		{
			m_pViewport->setView(overlayView);
			break;
		}
		case ToIdx(LayerID::Default):
		{
			m_pViewport->setView(worldView);
			break;
		}
		case ToIdx(LayerID::UI):
		{
			m_pViewport->setView(uiView);
			break;
		}
#ifdef DEBUGGING
		case ToIdx(LayerID::DebugWorld):
		case ToIdx(LayerID::TopWorld):
		{
			m_pViewport->setView(worldView);
			break;
		}
#endif
		}

		auto& vec = matrix[layer];
		for (auto& pPrimitive : vec)
		{
			if (!pPrimitive->IsDestroyed() && pPrimitive->m_renderState.bEnabled)
			{
				pPrimitive->Draw(*m_pViewport, alpha);
#if ENABLED(RENDER_STATS)
				++drawCalls;
#endif
			}
#if ENABLED(RENDER_STATS)

			if (pPrimitive->IsEnabled() && pPrimitive->IsStatic())
			{
				++statics;
			}
#endif
		}
	}
#if ENABLED(RENDER_STATS)
	g_renderStats.quadCount = g_renderStats._quadCount_Internal;
	g_renderStats.staticCount = statics;
	g_renderStats.drawCallCount = drawCalls;
	g_renderStats.dynamicCount = drawCalls - statics;
	++g_renderStats.renderFrame;
	// Update FPS
	{
		++frameCount;
		g_renderStats.lastRenderTime = Time::Now();
		if (Maths::Abs((fpsTime - g_renderStats.lastRenderTime).AsSeconds()) >= 1.0f)
		{
			fpsTime = g_renderStats.lastRenderTime;
			g_renderStats.framesPerSecond = frameCount;
			frameCount = 0;
		}
	}
#endif
}

void LERenderer::StopRenderThread()
{
	if (m_bRendering.load(std::memory_order_relaxed))
	{
		Stop();
	}
}

void LERenderer::Start()
{
	if (m_data.bStartThread)
	{
		m_pViewport->setActive(false);
		m_bRendering.store(true, std::memory_order_relaxed);
		LOG_D("[Renderer] Deactivated Viewport on this thread, starting render thread");
		m_threadHandle = OS::Threads::Spawn([&]() { Async_Run(m_data.threadStartDelay); });
	}
}

void LERenderer::Stop()
{
	m_bRendering.store(false, std::memory_order_relaxed);
	if (m_threadHandle)
	{
		OS::Threads::Join(m_threadHandle);
		m_pViewport->setActive(true);
	}
}

#ifdef DEBUGGING
TweakBool(bQuadsUseJobs, &Quads::s_bUSE_JOBS);
#endif
void LERenderer::Async_Run(Time startDelay)
{
	Assert(!OS::IsMainThread(), "Renderer::Async_Run() called from Main thread!");
#if ENABLED(PROFILER)
	static Time dt60Hz = Time::Seconds(1.0f / 60);
#endif
	m_pViewport->setActive(true);
	std::this_thread::sleep_for(std::chrono::milliseconds(startDelay.AsMilliseconds()));
	LOG_D("R[Renderer] Activated Viewport on this thread, starting render loop");
	while (m_bRendering.load(std::memory_order_relaxed))
	{
		if (m_swapMutex.try_lock())
		{
			Time renderElapsed = Time::Now() - LastSwapTime();
			Fixed alpha = Maths::ComputeAlpha(renderElapsed, m_data.tickRate);
			PROFILE_CUSTOM("RENDER", dt60Hz, Colour(219, 10, 87));
			RenderFrame(*this, alpha);
			PROFILE_STOP("RENDER");
			m_swapMutex.unlock();
			WaitForVsync();
		}
		else
		{
			// Yield if mutex is locked by Swap
			std::this_thread::yield();
		}
	}
	m_pViewport->setActive(false);
	LOG_D("R[Renderer] Deactivated Viewport on this thread, terminating render loop");
}

void LERenderer::WaitForVsync()
{
	Assert(m_pViewport, "Viewport is null!");
	m_pViewport->display();
}

#if defined(DEBUGGING)
void LERenderer::ModifyTickRate(Time newTickRate)
{
	m_data.tickRate = newTickRate;
}
#endif
} // namespace LittleEngine
