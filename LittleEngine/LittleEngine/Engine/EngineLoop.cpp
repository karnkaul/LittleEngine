#include "stdafx.h"
#include "Logger.h"
#include "EngineLoop.h"
#include "EngineService.h"
#include "OS.h"
#include "LittleEngine/GFX/GFX.h"
#include "LittleEngine/RenderLoop/RenderHeap.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/Game/GameSettings.h"
#if DEBUGGING
#include "LittleEngine/Physics/Collider.h"
#endif
#include "SFMLAPI/Windowing/SFWindow.h"
#include "Utils.h"
#include "Logger.h"

namespace LittleEngine
{
void EngineLoop::StopTicking()
{
	m_bStopTicking = true;
}

void EngineLoop::PreRun()
{
	SFEventLoop::PreRun();

	if (!m_bInit)
		Init();
	m_uAsyncRenderLoop = MakeUnique<AsyncRenderLoop>(*m_uSFWindow, m_gfxBuffer, m_tickRate, m_bRenderThread);
	m_uEngineService->m_pRenderLoop = m_uAsyncRenderLoop.get();
	m_uRenderHeap = MakeUnique<RenderHeap>(m_gfxBuffer);
	GFX::Init(*m_uSFWindow);
	m_uEngineService->PreRun();
}

void EngineLoop::Tick(Time dt)
{
	SFEventLoop::Tick(dt);

	UpdateInput();
	ReconcileRenderStates();
	Integrate(dt);
}

void EngineLoop::PostTick()
{
	SFEventLoop::PostTick();

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
	SFEventLoop::PostRun();

	// Block this thread until Rendering thread joins and AsyncRenderLoop gets destroyed
	m_uAsyncRenderLoop = nullptr;

	Services::Clear();
	Uninit();
	RenderCleanup();
}

void EngineLoop::OnPause(bool bPause)
{
	SFEventLoop::OnPause(bPause);

	if (bPause)
		m_uEngineService->Audio()->PauseAll();
	else
		m_uEngineService->Audio()->ResumeAll();
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
	// Swap buffers etc
	Vector2 cullBounds(m_windowData.windowSize.width, m_windowData.windowSize.height);
	GFXDataFrame dataFrame = Services::RHeap()->ConstructDataFrame();
	m_gfxBuffer.Lock_Swap(dataFrame.CollapseAndMove(), cullBounds);
}

void EngineLoop::Init()
{
#if !SHIPPING
	LOG_I("[EngineLoop] Initialising engine, loading config...");
	m_config.Load("_config.gd");
#endif
	Core::g_MinLogSeverity = m_config.GetLogLevel();
	m_bPauseOnFocusLoss = m_config.ShouldPauseOnFocusLoss();
	OS::Platform()->SetDesiredWorkerCount(m_config.GetNumGameThreads());

	m_uJobManager = MakeUnique<JobManager>();

	if (m_config.ShouldCreateRenderThread())
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
	Collider::s_debugShapeWidth = m_config.GetColliderBorderWidth();
#endif
	GameSettings* gameSettings = GameSettings::Instance();
	m_tickRate = Time::Seconds(1.0f / static_cast<f64>(m_config.GetTicksPerSecond()));
	m_maxFrameTime = Time::Milliseconds(m_config.GetMaxTickTimeMS());
	m_windowData.title = m_config.GetWindowTitle();
	m_windowData.viewSize = m_config.GetViewSize();
	u32 windowHeight = gameSettings->GetWindowHeight();
	u32 windowWidth = (m_windowData.viewSize.x.ToU32() * windowHeight) / m_windowData.viewSize.y.ToU32();
	m_windowData.windowSize = SFWindowSize(windowWidth, windowHeight);
	m_windowData.sfStyle = gameSettings->IsBordlerless() ? sf::Style::None : sf::Style::Close;
	m_uEngineService = MakeUnique<EngineService>();
	m_bInit = true;
}

void EngineLoop::Uninit()
{
#if !SHIPPING
	m_config.Save("_config.gd");
#endif
	m_uEngineService = nullptr;
	m_uJobManager = nullptr;
	m_bInit = false;
	LOG_I("[EngineLoop] Terminated, config saved.");
}
} // namespace LittleEngine
