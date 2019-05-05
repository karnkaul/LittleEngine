#include "stdafx.h"
#include "Logger.h"
#include "Utils.h"
#include "SFMLAPI/Windowing/SFWindow.h"
#include "EngineConfig.h"
#include "EngineLoop.h"
#include "EngineService.h"
#include "FatalEngineException.h"
#include "OS.h"
#include "LittleEngine/Audio/EngineAudio.h"
#include "LittleEngine/Debug/DebugProfiler.h"
#include "LittleEngine/Input/EngineInput.h"
#include "LittleEngine/Game/GameSettings.h"
#include "LittleEngine/GFX/GFX.h"
#include "LittleEngine/Jobs/JobManager.h"
#include "LittleEngine/RenderLoop/AsyncRenderLoop.h"
#include "LittleEngine/RenderLoop/RenderFactory.h"
#include "LittleEngine/Services/Services.h"
#if DEBUGGING
#include "LittleEngine/Physics/Collider.h"
#endif
#include "LittleEngine/Game/World/WorldStateMachine.h"

namespace LittleEngine
{
Time g_maxFrameTime;

EngineLoop::~EngineLoop() = default;

void EngineLoop::PreRun()
{
	if (!m_bInit)
	{
		Init();
	}
	m_uRenderFactory = MakeUnique<RenderFactory>();
	m_uAsyncRenderLoop = MakeUnique<AsyncRenderLoop>(*m_uSFWindow, m_tickRate, m_bRenderThread);
	m_uEngineService->m_pRenderLoop = m_uAsyncRenderLoop.get();
	GFX::Init(*m_uSFWindow);
	m_uEngineService->PreRun();
}

void EngineLoop::PreTick()
{
	m_uRenderFactory->Reconcile();
	m_uEngineService->UpdateInput(GetInputDataFrame());
}

bool EngineLoop::Tick(Time dt)
{
	PROFILE_START("Tick", Colour(127, 0, 255, 255));
	bool bYield = m_uEngineService->Tick(dt);
	PROFILE_STOP("Tick");
	return bYield;
}

void EngineLoop::FinishFrame()
{
	m_uEngineService->PreFinishFrame();
	m_uAsyncRenderLoop->m_bPauseRendering.store(true, std::memory_order_release);
	m_uRenderFactory->Lock_Swap();
	m_uAsyncRenderLoop->m_bPauseRendering.store(false, std::memory_order_release);

	if (!m_bRenderThread)
	{
		Time renderElapsed = Time::Now() - m_uRenderFactory->GetLastSwapTime();
		Fixed alpha = Maths::ComputeAlpha(renderElapsed, m_tickRate);
		m_uAsyncRenderLoop->Render(*m_uRenderFactory, alpha);
	}

	m_bStopTicking = m_uEngineService->m_bTerminating;
}

void EngineLoop::PostRun()
{
	// Block this thread until Rendering thread joins and AsyncRenderLoop gets destroyed
	m_uAsyncRenderLoop = nullptr;

	Services::Clear();
	Uninit();
	m_uRenderFactory = nullptr;
}

void EngineLoop::OnPause(bool bPause)
{
	ASFEventLoop::OnPause(bPause);

	if (bPause)
	{
		m_uEngineService->Audio()->PauseAll();
	}
	else
	{
		m_uEngineService->Audio()->ResumeAll();
	}
}

UPtr<EngineLoop> EngineLoop::Create()
{
	UPtr<EngineLoop> uEngine = MakeUnique<EngineLoop>();
	if (uEngine->Start())
	{
		return uEngine;
	}
	return nullptr;
}
	

bool EngineLoop::Start()
{
	if (m_bInit)
	{
		LOG_W("[EngineLoop] Engine already started");
		return true;
	}
	return m_bInit = Init();
}

WorldStateMachine* EngineLoop::Worlds() const
{
	return Services::Engine()->Worlds();
}

bool EngineLoop::Init()
{
	m_uConfig = MakeUnique<EngineConfig>();
#if !SHIPPING
	LOG_I("[EngineLoop] Initialising engine, loading config...");
	m_uConfig->Load("_config.gd");
#endif
	Core::g_MinLogSeverity = m_uConfig->GetLogLevel();
	m_bPauseOnFocusLoss = m_uConfig->ShouldPauseOnFocusLoss();
	OS::Platform()->SetDesiredWorkerCount(m_uConfig->GetNumGameThreads());

	m_uJobManager = MakeUnique<JobManager>();
	m_bRenderThread = m_uConfig->ShouldCreateRenderThread();
	if (m_bRenderThread)
	{
		if (!OS::Platform()->CanCreateSystemThread())
		{
			LOG_W(
				"Insufficient threads to create render thread!\n!ERROR! Async Renderer not "
				"available!");
			m_bRenderThread = false;
		}
	}

#if DEBUGGING
	Collider::s_debugShapeWidth = m_uConfig->GetColliderBorderWidth();
#endif
	Vector2 viewSize = m_uConfig->GetViewSize();
	GameSettings* gameSettings = GameSettings::Instance();
	m_tickRate = Time::Seconds(1.0f / static_cast<f32>(m_uConfig->GetTicksPerSecond()));
	m_maxFrameTime = Time::Milliseconds(m_uConfig->GetMaxTickTimeMS());
	m_cullBounds = gameSettings->GetCullBounds(viewSize);
	m_uSFWindow->SetData(SFWindowData(gameSettings->GetWindowSize(viewSize), viewSize,
									  m_uConfig->GetWindowTitle(), gameSettings->GetWindowStyle()));
	try
	{
		m_uEngineService = MakeUnique<EngineService>(*this);
	}
	catch (const FatalEngineException& /*e*/)
	{
		LOG_E("ERROR! Could not initialise Engine Service!");
		return false;
	}
	
	g_maxFrameTime = m_maxFrameTime;
	return true;
}

void EngineLoop::Uninit()
{
#if !SHIPPING
	m_uConfig->Save("_config.gd");
#endif
	GFX::Cleanup();
	m_uEngineService = nullptr;
	m_uJobManager = nullptr;
	m_bInit = false;
	LOG_I("[EngineLoop] Terminated, config saved.");
}
} // namespace LittleEngine
