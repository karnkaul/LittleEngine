#include "stdafx.h"
#include "Logger.h"
#include "Utils.h"
#include "SFMLAPI/Windowing/SFWindow.h"
#include "EngineConfig.h"
#include "EngineLoop.h"
#include "EngineService.h"
#include "OS.h"
#include "LittleEngine/Audio/EngineAudio.h"
#include "LittleEngine/Input/EngineInput.h"
#include "LittleEngine/Game/GameSettings.h"
#include "LittleEngine/GFX/GFX.h"
#include "LittleEngine/Jobs/JobManager.h"
#include "LittleEngine/RenderLoop/AsyncRenderLoop.h"
#include "LittleEngine/RenderLoop/RenderHeap.h"
#include "LittleEngine/Services/Services.h"
#if DEBUGGING
#include "LittleEngine/Physics/Collider.h"
#endif

namespace LittleEngine
{
EngineLoop::~EngineLoop() = default;

void EngineLoop::StopTicking()
{
	m_bStopTicking = true;
}

void EngineLoop::PreRun()
{
	if (!m_bInit)
	{
		Init();
	}
	m_uAsyncRenderLoop = MakeUnique<AsyncRenderLoop>(*m_uSFWindow, m_gfxBuffer, m_tickRate, m_bRenderThread);
	m_uEngineService->m_pRenderLoop = m_uAsyncRenderLoop.get();
	m_uRenderHeap = MakeUnique<RenderHeap>(m_gfxBuffer);
	GFX::Init(*m_uSFWindow);
	m_uEngineService->PreRun();
}

void EngineLoop::Tick(Time dt)
{
	UpdateInput();
	ReconcileRenderStates();
	Integrate(dt);
}

void EngineLoop::PostTick()
{
	m_uEngineService->PostTick();
	SwapGFXBuffer();
	m_uEngineService->PostBufferSwap();

	if (!m_bRenderThread)
	{
		m_uAsyncRenderLoop->Render(m_gfxBuffer);
		m_uAsyncRenderLoop->Display();
	}

	m_bStopTicking = m_uEngineService->m_bTerminate;
}

void EngineLoop::PostRun()
{
	// Block this thread until Rendering thread joins and AsyncRenderLoop gets destroyed
	m_uAsyncRenderLoop = nullptr;

	Services::Clear();
	Uninit();
	RenderCleanup();
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

UPtr<EngineLoop> LittleEngine::EngineLoop::Create()
{
	UPtr<EngineLoop> uEngine = MakeUnique<EngineLoop>();
	uEngine->Start();
	return uEngine;
}

void EngineLoop::Start()
{
	if (m_bInit)
	{
		LOG_W("[EngineLoop] Engine already started");
		return;
	}
	Init();
}

void EngineLoop::UpdateInput()
{
	m_uEngineService->UpdateInput(GetInputDataFrame());
}

void EngineLoop::Integrate(Time dt)
{
	m_uEngineService->Tick(dt);
}

void EngineLoop::RenderCleanup()
{
	GFX::Cleanup();
	Services::RHeap()->DestroyAll();
	m_uRenderHeap = nullptr;
}

void EngineLoop::ReconcileRenderStates()
{
	Services::RHeap()->Reconcile();
}

void EngineLoop::SwapGFXBuffer()
{
	m_gfxBuffer.Lock_Swap(Services::RHeap()->ConstructDataFrame(), m_cullBounds);
}

void EngineLoop::Init()
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

	if (m_uConfig->ShouldCreateRenderThread())
	{
		if (!OS::Platform()->CanCreateSystemThread())
		{
			LOG_W(
				"Insufficient threads to create render thread!\n!ERROR! Async Renderer not "
				"available!");
		}
		else
		{
			OS::Platform()->SetCreatingRenderThread();
			m_bRenderThread = true;
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
	m_uEngineService = MakeUnique<EngineService>(*this);
	m_bInit = true;
}

void EngineLoop::Uninit()
{
#if !SHIPPING
	m_uConfig->Save("_config.gd");
#endif
	m_uEngineService = nullptr;
	m_uJobManager = nullptr;
	m_bInit = false;
	LOG_I("[EngineLoop] Terminated, config saved.");
}
} // namespace LittleEngine
