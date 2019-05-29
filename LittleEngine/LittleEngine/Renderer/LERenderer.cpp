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
//#include "LittleEngine/Debug/DebugProfiler.h"

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
	LOG_I("[Renderer] Activated SFViewport on this thread and recreated it");
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
	return m_pViewport ? m_pViewport->GetViewSize() : Vector2::Zero;
}

Vector2 LERenderer::Project(Vector2 nPos, bool bPreClamp) const
{
	return m_pViewport ? m_pViewport->Project(nPos, bPreClamp) : nPos;
}

SFPrimitive* LERenderer::New(LayerID layer)
{
	return m_uFactory ? m_uFactory->New(layer) : nullptr;
}

Time LERenderer::GetLastSwapTime() const
{
	Assert(m_uFactory, "RenderFactory is null!");
	return m_uFactory->GetLastSwapTime();
}

void LERenderer::Lock_Swap()
{
	Assert(m_uFactory, "RenderFactory is null!");
	m_bPauseRendering.store(true, std::memory_order_release);
	m_uFactory->Lock_Swap();
	m_bPauseRendering.store(false, std::memory_order_release);
}

void LERenderer::Reconcile()
{
	Assert(m_uFactory, "RenderFactory is null!");
	m_uFactory->Reconcile();
}

void LERenderer::Render(Fixed alpha)
{
	Assert(m_uFactory, "RenderFactory is null!");
	RenderFrame(*m_uFactory, alpha);
}

void LERenderer::StopRendering()
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
		LOG_I("[Renderer] Deactivated SFViewport on this thread, starting render thread");
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
	m_pViewport->setActive(true);
	m_pViewport->setVerticalSyncEnabled(true);
	std::this_thread::sleep_for(std::chrono::milliseconds(startDelay.AsMilliseconds()));
	LOG_I("R[Renderer] Activated SFViewport on this thread, starting render loop");
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
			Assert(m_uFactory, "RenderFactory is null!");
			RenderFrame(*m_uFactory, alpha);
		}
	}
	m_pViewport->setActive(false);
	LOG_I("R[Renderer] Deactivated SFViewport on this thread, terminating render loop");
}

#if DEBUGGING
void LERenderer::ModifyTickRate(Time newTickRate)
{
	m_data.tickRate = newTickRate;
}
#endif
} // namespace LittleEngine
