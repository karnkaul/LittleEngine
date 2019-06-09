#include "stdafx.h"
#include "Core/Asserts.h"
#include "Core/Logger.h"
#include "SFMLAPI/Rendering/ISFRenderBuffer.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"
#include "SFMLAPI/Viewport/SFViewportData.h"
#include "SFMLAPI/Viewport/SFViewport.h"
#include "LERenderer.h"
#include "RenderFactory.h"
#include "LittleEngine/OS.h"
#include "LittleEngine/Debug/Profiler.h"

namespace LittleEngine 
{
LERenderer::LERenderer(SFViewport& sfViewport, RendererData data)
	: SFRenderer(sfViewport), m_data(data)
{
	SFViewportSize max = SFViewport::GetMaxSize();
	Vec<u32> heights = {360, 540, 720, 900, 1080, 1440, 2160};
	m_viewportSizes.clear();
	sf::Vector2u w = sfViewport.getSize();
	for (auto height : heights)
	{
		u32 width = (w.x * height) / w.y;
		if (width <= max.width && height <= max.height)
		{
			m_viewportSizes[height] = {width, height};
		}
	}

	m_uFactory = MakeUnique<RenderFactory>();	
	Start();
}

LERenderer::~LERenderer()
{
	if (m_bRendering.load(std::memory_order_relaxed))
	{
		Stop();
	}
	m_uFactory = nullptr;
}

void LERenderer::RecreateViewport(SFViewportRecreateData data)
{
	Assert(OS::IsMainThread(), "Renderer::RecreateWindow() not called from Main thread!");
	Stop();
	m_pViewport->setActive(true);
	m_pViewport->Destroy();
	m_pViewport->OverrideData(std::move(data));
	m_pViewport->Create();
	LOG_D("[Renderer] Activated SFViewport on this thread and recreated it");
	Start();
}

const Map<u32, SFViewportSize>& LERenderer::GetValidViewportSizes() const
{
	return m_viewportSizes;
}

SFViewportSize* LERenderer::TryGetViewportSize(u32 height)
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

SFPrimitive* LERenderer::New(LayerID layer)
{
	return m_uFactory ? m_uFactory->New(layer) : nullptr;
}

Time LERenderer::GetLastSwapTime() const
{
	return m_uFactory->GetLastSwapTime();
}

void LERenderer::Lock_Swap()
{
#if ENABLED(RENDER_STATS) 
	g_renderData.rendersPerFrame.store(0, std::memory_order_relaxed);
#endif
	PROFILE_CUSTOM("LOCK", m_data.tickRate, Colour(255, 72, 0)); 
	m_bPauseRendering.store(true, std::memory_order_release);
	std::lock_guard<std::mutex> lock(m_uFactory->m_mutex);
	PROFILE_STOP("LOCK");

	PROFILE_CUSTOM("SWAP", m_data.tickRate, Colour(255, 72, 0));
	m_uFactory->Swap();
	m_bPauseRendering.store(false, std::memory_order_release);
	PROFILE_STOP("SWAP");
}

void LERenderer::Reconcile()
{
	m_uFactory->Reconcile();
}

void LERenderer::Render(Fixed alpha)
{
	RenderFrame(*m_uFactory, alpha);
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
		LOG_D("[Renderer] Deactivated SFViewport on this thread, starting render thread");
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
	LOG_D("R[Renderer] Activated SFViewport on this thread, starting render loop");
	while (m_bRendering.load(std::memory_order_relaxed))
	{
		if (m_bPauseRendering.load(std::memory_order_acquire))
		{
			std::this_thread::yield();
		}
		else
		{
			Time renderElapsed = Time::Now() - m_uFactory->GetLastSwapTime();
			Fixed alpha = Maths::ComputeAlpha(renderElapsed, m_data.tickRate);
			PROFILE_CUSTOM("RENDER", dt60Hz, Colour(219, 10, 87));
			RenderFrame(*m_uFactory, alpha);
			PROFILE_STOP("RENDER");
		}
	}
	m_pViewport->setActive(false);
	LOG_D("R[Renderer] Deactivated SFViewport on this thread, terminating render loop");
}

#if DEBUGGING
void LERenderer::ModifyTickRate(Time newTickRate)
{
	m_data.tickRate = newTickRate;
}
#endif
} // namespace LittleEngine
