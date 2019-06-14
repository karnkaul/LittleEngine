#include "stdafx.h"
#include "Core/FileLogger.h"
#include "Core/Jobs.h"
#include "SFMLAPI/System/SFAssets.h"
#include "SFMLAPI/Viewport/SFViewport.h"
#include "SFMLAPI/Viewport/SFViewportData.h"
#include "LEContext.h"
#include "LittleEngine/Audio/LEAudio.h"
#include "LittleEngine/Debug/Profiler.h"
#include "LittleEngine/Debug/Tweakable.h"
#include "LittleEngine/OS.h"
#include "LittleEngine/Input/LEInput.h"
#include "LittleEngine/Renderer/LERenderer.h"
#include "LittleEngine/Repository/LERepository.h"

#include "SFMLAPI/Rendering/Primitives/Primitive.h"

namespace LittleEngine
{
TweakBool(asyncRendering, nullptr);

LEContext::LEContext(LEContextData data) : m_data(std::move(data))
{
#if ENABLED(TWEAKABLES) 
	asyncRendering.BindCallback([&](const String& val) { 
		bool bEnable = Strings::ToBool(val);
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
	m_bTerminating = false;
	m_uViewport = MakeUnique<SFViewport>();
	m_uViewport->SetData(std::move(m_data.viewportData));
	m_uViewport->Create();

	m_uInput = MakeUnique<LEInput>(*this);

#if ENABLED(RENDER_STATS)
	g_renderData.tickRate = m_data.tickRate;
#endif
	RendererData rData{m_data.tickRate, Time::Milliseconds(20), m_uViewport.get()};
	m_uRenderer = MakeUnique<LERenderer>(*m_uViewport, rData);
}

LEContext::~LEContext()
{
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

Vector2 LEContext::GetViewSize() const
{
	return m_uRenderer->GetViewSize();
}

Time LEContext::GetMaxFrameTime() const
{
	return m_data.maxFrameTime;
}

bool LEContext::TrySetViewportSize(u32 height)
{
	m_oNewViewportSize.emplace(m_uRenderer->TryGetViewportSize(height));
	if (!m_oNewViewportSize)
	{
		LOG_W("[LEContext] No resolution that matches given height: %d", height);
		return false;
	}
	return true;
}

void LEContext::SetWindowStyle(SFViewportStyle newStyle)
{
	m_oNewViewportStyle.emplace(std::move(newStyle));
}

void LEContext::Terminate()
{
	if (!Core::Jobs::AreWorkersIdle())
	{
		Assert(false, "Terminate() called while job workers are active!");
		LOG_E("[LEContext] Terminate() called while job workers are active!");
		m_bWaitingToTerminate = true;
	}
	else
	{
		m_bTerminating = true;
		m_uRenderer->StopRenderThread();
	}
}

void LEContext::PollInput()
{
	SFViewportEventType sfEvent = m_inputHandler.PollEvents(*m_uViewport);
	m_uInput->TakeSnapshot(m_inputHandler.GetFrameInputData());
	switch (sfEvent)
	{
	case SFViewportEventType::Closed:
		Terminate();
		return;

	default:
		break;
	}
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
		m_bTerminating = Core::Jobs::AreWorkersIdle();
		if (!m_bTerminating)
		{
			LOG_W("[LEContext] Engine termination blocked by JobManager...");
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
	if (m_oNewViewportSize)
	{
		LOG_I("[LEContext] Set viewport size to: %dx%d", (*m_oNewViewportSize)->width, (*m_oNewViewportSize)->height);
		m_uRenderer->RecreateViewport(SFViewportRecreateData(std::move(**m_oNewViewportSize)));
		m_oNewViewportSize.reset();
	}

	if (m_oNewViewportStyle)
	{
		LOG_I("[LEContext] Changed viewport style");
		m_uRenderer->RecreateViewport(SFViewportRecreateData(*m_oNewViewportStyle));
		m_oNewViewportStyle.reset();
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
}

#if DEBUGGING
void LEContext::ModifyTickRate(Time newTickRate)
{
	g_renderData.tickRate = newTickRate;
	m_uRenderer->ModifyTickRate(newTickRate);
}
#endif

void LEContext::PollEvents()
{
	SFViewportEventType windowEvent = m_inputHandler.PollEvents(*m_uViewport);
	switch (windowEvent)
	{
	case SFViewportEventType::Closed:
		Terminate();
		break;

	case SFViewportEventType::LostFocus:
		if (m_data.bPauseOnFocusLoss)
		{
			m_bPauseTicking = true;
			g_pAudio->PauseAll();
		}
		break;

	case SFViewportEventType::GainedFocus:
		if (m_data.bPauseOnFocusLoss)
		{
			m_bPauseTicking = false;
			g_pAudio->ResumeAll();
		}
		break;

	default:
		break;
	}
}
} // namespace LittleEngine
