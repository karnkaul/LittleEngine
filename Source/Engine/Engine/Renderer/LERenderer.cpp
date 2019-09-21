#include "Core/Asserts.h"
#include "Core/Logger.h"
#include "Core/OS.h"
#include "Core/Utils.h"
#include "SFMLAPI/Rendering/IRenderBuffer.h"
#include "SFMLAPI/Rendering/Primitives/Quads.h"
#include "SFMLAPI/Viewport/ViewportData.h"
#include "SFMLAPI/Viewport/Viewport.h"
#include "LERenderer.h"
#include "Engine/Debug/Profiler.h"
#include "Engine/Debug/Tweakable.h"

namespace LittleEngine
{
LERenderer::LERenderer(Viewport& viewport, RendererData data) : Renderer(viewport), m_data(data)
{
#if ENABLED(RENDER_STATS)
	extern RenderData g_renderData;
	g_renderData.fpsMax = m_data.maxFPS;
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
	PROFILE_CUSTOM("LOCK", m_data.tickRate, Colour(255, 72, 0));
	Lock lock(m_swapMutex);
	PROFILE_STOP("LOCK");

	PROFILE_CUSTOM("SWAP", m_data.tickRate, Colour(255, 72, 0));
	Swap();
	PROFILE_STOP("SWAP");
}

void LERenderer::Render(Fixed alpha)
{
	{
		Lock lock(m_swapMutex);
		RenderFrame(*this, alpha);
	}
	WaitForVsync();
}

void LERenderer::StopRenderThread()
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

#if defined(DEBUGGING)
void LERenderer::ModifyTickRate(Time newTickRate)
{
	m_data.tickRate = newTickRate;
}
#endif
} // namespace LittleEngine
