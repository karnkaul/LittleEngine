#include "Core/Jobs.h"
#include "SFMLAPI/System/Assets.h"
#include "SFMLAPI/Viewport/Viewport.h"
#include "SFMLAPI/Viewport/ViewportData.h"
#include "LEContext.h"
#include "Engine/Audio/LEAudio.h"
#include "Engine/Debug/Profiler.h"
#include "Engine/Debug/Tweakable.h"
#include "Engine/Input/LEInput.h"
#include "Engine/Renderer/LERenderer.h"
#include "Engine/Repository/LERepository.h"
#include "Engine/GFX.h"
#include "SFMLAPI/Rendering/Primitives/Primitive.h"

namespace LittleEngine
{
TweakBool(asyncRendering, nullptr);

LEContext::LEContext(LEContextData data) : m_data(std::move(data))
{
#if ENABLED(TWEAKABLES)
	asyncRendering.BindCallback([&](VString val) {
		bool bEnable = Strings::ToBool(String(val));
		if (bEnable)
		{
			if (!m_uRenderer->IsRunningRenderThread())
			{
				m_uViewport->setActive(false);
				m_uRenderer->m_data.bStartThread = true;
				m_uRenderer->Start();
				m_data.bRenderThread = true;
				LOG_I("[Context] Spawned render thread");
			}
			else
			{
				LOG_W("[Context] Render thread already running");
			}
		}
		else
		{
			if (m_uRenderer->IsRunningRenderThread())
			{
				m_uRenderer->StopRenderThread();
				m_uViewport->setActive(true);
				m_data.bRenderThread = false;
				m_uRenderer->m_bRendering.store(true, std::memory_order_relaxed);
				LOG_I("[Context] Destroyed render thread");
			}
			else
			{
				LOG_W("[Context] Render thread not running");
			}
		}
	});
#endif
	auto maxFPS = Maths::Clamp(OS::ScreenRefreshRate() * 2, 60U, 300U);
	m_bTerminating = false;
	m_uViewport = MakeUnique<Viewport>();
	m_uViewport->SetData(std::move(m_data.viewportData));
	m_uViewport->Create(maxFPS);

	m_uInput = MakeUnique<LEInput>(*this, std::move(m_data.inputMap));

#if ENABLED(RENDER_STATS)
	g_renderData.tickRate = m_data.tickRate;
#endif
	RendererData rData{m_data.tickRate, Time::Milliseconds(20), maxFPS, m_data.bRenderThread};
	m_uRenderer = MakeUnique<LERenderer>(*m_uViewport, rData);
}

LEContext::~LEContext()
{
	m_uRenderer->StopRenderThread();
	while (!Core::Jobs::AreWorkersIdle())
	{
		LOG_E("[LEContext] Engine destruction blocked by JobManager...");
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	m_uInput = nullptr;
	m_uRenderer = nullptr;
	m_uViewport = nullptr;
}

bool LEContext::IsTerminating() const
{
	return m_bTerminating;
}

bool LEContext::IsPaused() const
{
	return m_bPauseTicking;
}

LEInput* LEContext::Input() const
{
	return m_uInput.get();
}

LERenderer* LEContext::Renderer() const
{
	return m_uRenderer.get();
}

Time LEContext::MaxFrameTime() const
{
	return m_data.maxFrameTime;
}

bool LEContext::TrySetViewportSize(u32 height)
{
	m_oNewViewportSize.emplace(g_pGFX->TryGetViewportSize(height));
	if (!m_oNewViewportSize)
	{
		LOG_W("[LEContext] No resolution that matches given height: %d", height);
		return false;
	}
	return true;
}

void LEContext::SetWindowStyle(ViewportStyle newStyle)
{
	m_oNewViewportStyle.emplace(std::move(newStyle));
}

void LEContext::Terminate()
{
	if (g_pRepository->IsBusy())
	{
		Assert(false, "Terminate() called while repository is busy!");
		LOG_E("[LEContext] Terminate() called while LERepository is busy!");
		m_bWaitingToTerminate = true;
	}
	else
	{
		m_bTerminating = true;
	}
}

void LEContext::PollInput()
{
	ViewportEventType sfEvent = m_uInput->PollEvents(*m_uViewport);
	m_uInput->TakeSnapshot();
	switch (sfEvent)
	{
	case ViewportEventType::Closed:
		Terminate();
		return;

	default:
		break;
	}
}

void LEContext::FireInput()
{
	if (!m_bTerminating)
	{
		m_uInput->FireCallbacks();
	}
}

bool LEContext::Update()
{
	Core::Jobs::Update();
	if (!m_bWaitingToTerminate)
	{
		return m_bTerminating;
	}
	else
	{
		m_uRenderer->StopRenderThread();
		m_bTerminating = Core::Jobs::AreWorkersIdle();
		if (!m_bTerminating)
		{
			LOG_W("[LEContext] Engine termination blocked by JobManager... Undefined Behaviour!");
		}
		return true;
	}
}
void LEContext::StartFrame()
{
	m_uRenderer->Reconcile();
}

void LEContext::SubmitFrame()
{
	bool bRecreateViewport = false;
	ViewportRecreateData data;
	if (m_oNewViewportSize)
	{
		LOG_I("[LEContext] Set viewport size to: %dx%d", (*m_oNewViewportSize)->width, (*m_oNewViewportSize)->height);
		data.oViewportSize = **m_oNewViewportSize;
		g_pGFX->m_viewportHeight = ToS32((*m_oNewViewportSize)->height);
		bRecreateViewport = true;
		m_oNewViewportSize.reset();
	}
	if (m_oNewViewportStyle)
	{
		LOG_I("[LEContext] Changed viewport style");
		data.oStyle = *m_oNewViewportStyle;
		bRecreateViewport = true;
		m_oNewViewportStyle.reset();
	}
	if (bRecreateViewport)
	{
		m_uRenderer->RecreateViewport(std::move(data));
		g_pGFX->Init();
	}
	m_uRenderer->Lock_Swap();
	if (!m_data.bRenderThread)
	{
#if ENABLED(PROFILER)
		static Time dt60Hz = Time::Seconds(1.0f / 60);
#endif
		PROFILE_CUSTOM("RENDER", dt60Hz, Colour(219, 10, 87));
		m_uRenderer->Render(Fixed::One);
		PROFILE_STOP("RENDER");
	}
#if ENABLED(RENDER_STATS)
	++g_renderData.gameFrame;
#endif

	m_onSubmitted();
	m_onSubmitted.Clear();
}

LEContext::OnSubmit::Token LEContext::RegisterOnSubmitted(OnSubmit::Callback onFrameSubmitted)
{
	return m_onSubmitted.Register(onFrameSubmitted);
}

#if defined(DEBUGGING)
void LEContext::ModifyTickRate(Time newTickRate)
{
	g_renderData.tickRate = newTickRate;
	m_uRenderer->ModifyTickRate(newTickRate);
}
#endif
} // namespace LittleEngine
