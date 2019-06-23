#include "stdafx.h"
#include "Core/Asserts.h"
#include "Core/Logger.h"
#include "SFMLAPI/Rendering/IRenderBuffer.h"
#include "SFMLAPI/Viewport/ViewportData.h"
#include "SFMLAPI/Viewport/Viewport.h"
#include "LERenderer.h"
#include "LittleEngine/OS.h"
#include "LittleEngine/Debug/Profiler.h"

namespace LittleEngine 
{
LERenderer::LERenderer(Viewport& viewport, RendererData data)
	: Renderer(viewport), m_data(data)
{
	ViewportSize max = Viewport::GetMaxSize();
	Vec<u32> heights = {360, 540, 720, 900, 1080, 1440, 2160};
	m_viewportSizes.clear();
	sf::Vector2u w = viewport.getSize();
	for (auto height : heights)
	{
		u32 width = (w.x * height) / w.y;
		if (width <= max.width && height <= max.height)
		{
			m_viewportSizes[height] = {width, height};
		}
	}

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
	m_pViewport->Create();
	LOG_D("[Renderer] Activated Viewport on this thread and recreated it");
	Start();
}

const Map<u32, ViewportSize>& LERenderer::GetValidViewportSizes() const
{
	return m_viewportSizes;
}

ViewportSize* LERenderer::TryGetViewportSize(u32 height)
{
	auto search = m_viewportSizes.find(height);
	if (search != m_viewportSizes.end())
	{
		return &search->second;
	}
	return nullptr;
}

Vector2 LERenderer::GetViewSize() const
{
	return m_pViewport->GetViewSize();
}

Vector2 LERenderer::Project(Vector2 nPos, bool bPreClamp) const
{
	return m_pViewport->Project(nPos, bPreClamp);
}

void LERenderer::Lock_Swap()
{
#if ENABLED(RENDER_STATS) 
	g_renderData.rendersPerFrame.store(0, std::memory_order_relaxed);
#endif
	PROFILE_CUSTOM("LOCK", m_data.tickRate, Colour(255, 72, 0)); 
	m_bPauseRendering.store(true, std::memory_order_release);
	Lock lock(m_mutex);
	PROFILE_STOP("LOCK");

	PROFILE_CUSTOM("SWAP", m_data.tickRate, Colour(255, 72, 0));
	Swap();
	m_bPauseRendering.store(false, std::memory_order_release);
	PROFILE_STOP("SWAP");
}

void LERenderer::Render(Fixed alpha)
{
	RenderFrame(*this, alpha);
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

void LERenderer::Async_Run(Time startDelay)
{
	Assert(!OS::IsMainThread(), "Renderer::Async_Run() called from Main thread!");
#if ENABLED(PROFILER)
	static Time dt60Hz = Time::Seconds(1.0f / 60);
#endif
	m_pViewport->setActive(true);
	m_pViewport->setVerticalSyncEnabled(true);
	std::this_thread::sleep_for(std::chrono::milliseconds(startDelay.AsMilliseconds()));
	LOG_D("R[Renderer] Activated Viewport on this thread, starting render loop");
	while (m_bRendering.load(std::memory_order_relaxed))
	{
		if (m_bPauseRendering.load(std::memory_order_acquire))
		{
			std::this_thread::yield();
		}
		else
		{
			Time renderElapsed = Time::Now() - GetLastSwapTime();
			Fixed alpha = Maths::ComputeAlpha(renderElapsed, m_data.tickRate);
			PROFILE_CUSTOM("RENDER", dt60Hz, Colour(219, 10, 87));
			RenderFrame(*this, alpha);
			PROFILE_STOP("RENDER");
		}
	}
	m_pViewport->setActive(false);
	LOG_D("R[Renderer] Deactivated Viewport on this thread, terminating render loop");
}

#if DEBUGGING
void LERenderer::ModifyTickRate(Time newTickRate)
{
	m_data.tickRate = newTickRate;
}
#endif
} // namespace LittleEngine
