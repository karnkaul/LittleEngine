#include "stdafx.h"
#include "AsyncFileLogger.h"
#include "SFMLAPI/Input/SFInputStateMachine.h"
#include "SFMLAPI/System/SFAssets.h"
#include "SFMLAPI/Windowing/SFWindow.h"
#include "SFMLAPI/Windowing/SFWindowData.h"
#include "EngineService.h"
#include "LittleEngine/Audio/EngineAudio.h"
#include "LittleEngine/Debug/DebugProfiler.h"
#include "LittleEngine/Debug/Console/DebugConsole.h"
#include "LittleEngine/Engine/EngineLoop.h"
#include "LittleEngine/Engine/OS.h"
#include "LittleEngine/Game/GameSettings.h"
#include "LittleEngine/Game/World/WorldStateMachine.h"
#include "LittleEngine/GFX/GFX.h"
#include "LittleEngine/Input/EngineInput.h"
#include "LittleEngine/Jobs/JobManager.h"
#include "LittleEngine/RenderLoop/AsyncRenderLoop.h"
#include "LittleEngine/Repository/EngineRepository.h"
#include "LittleEngine/Services/Services.h"

namespace LittleEngine
{
#if DEBUGGING
using namespace Debug;
#endif

namespace
{
SFWindowSize* pNewWindowSize = nullptr;
bool bChangeResolution = false;

SFWindowStyle newWindowStyle = SFWindowStyle::Default;
bool bChangeWindowStyle = false;
} // namespace

EngineService::EngineService(EngineLoop& engineLoop) : m_pEngineLoop(&engineLoop)
{
	if (!OS::Platform()->CanCreateSystemThread() || Services::Jobs()->AvailableEngineThreads() < 1)
	{
		LOG_W(
			"Insufficient workers to create persistent AsyncFileLogger job."
			"\n!ERROR! File logging is not available!");
	}
	else
	{
		OS::Platform()->SetCreatingLoggerThread();
		m_uFileLogger = MakeUnique<AsyncFileLogger>("Debug.log");
	}
	m_uEngineInput = MakeUnique<EngineInput>();
	m_uEngineRepository = MakeUnique<EngineRepository>("GameAssets.cooked", "GameAssets");
	m_uWorldStateMachine = MakeUnique<WorldStateMachine>();
	m_uEngineAudio = MakeUnique<EngineAudio>();
	Time::Reset();
	Services::ProvideEngine(*this);
}

EngineService::~EngineService()
{
#if ENABLED(PROFILER)
	Profiler::Cleanup();
#endif
#if ENABLED(CONSOLE)
	Console::Cleanup();
#endif
	m_uWorldStateMachine = nullptr;
	m_uEngineInput = nullptr;
	m_uEngineRepository = nullptr;
	m_uEngineAudio = nullptr;
	Services::UnprovideEngine(*this);
	LOG_I("Logging terminated");
	if (m_uFileLogger)
	{
		m_uFileLogger = nullptr;
		OS::Platform()->ReleaseLoggerThread();
	}
}

WorldStateMachine* EngineService::Worlds() const
{
	return m_uWorldStateMachine.get();
}

EngineRepository* EngineService::Repository() const
{
	return m_uEngineRepository.get();
}

EngineInput* EngineService::Input() const
{
	return m_uEngineInput.get();
}

EngineAudio* EngineService::Audio() const
{
	return m_uEngineAudio.get();
}

void EngineService::TrySetWindowSize(u32 height)
{
	pNewWindowSize = GFX::TryGetWindowSize(height);
	if (pNewWindowSize)
	{
		bChangeResolution = true;
	}
	else
	{
		LOG_W("[EngineService] No resolution that matches given height: %d", height);
	}
}

void EngineService::SetWindowStyle(SFWindowStyle newStyle)
{
	bChangeWindowStyle = true;
	newWindowStyle = newStyle;
}

void EngineService::Terminate()
{
	m_bTerminating = true;
}

void EngineService::PreRun()
{
	m_bTerminating = false;
#if ENABLED(CONSOLE)
	Console::Init();
#endif
#if ENABLED(PROFILER)
	Profiler::Init(std::this_thread::get_id());
#endif
	m_uWorldStateMachine->Start("Manifest.amf");
}

void EngineService::UpdateInput(const SFInputDataFrame& inputDataFrame)
{
	m_uEngineInput->TakeSnapshot(inputDataFrame);
	m_uEngineInput->FireCallbacks();
}

bool EngineService::Tick(Time dt)
{
	Services::Jobs()->Tick(dt);
	m_uEngineRepository->Tick(dt);
	bool bWorldStateChanged = m_uWorldStateMachine->Tick(dt);
	m_uEngineAudio->Tick(dt);
#if ENABLED(CONSOLE)
	Console::Tick(dt);
#endif
#if ENABLED(PROFILER)
	Profiler::Tick(dt);
#endif
	return bWorldStateChanged || m_bTerminating;
}

void EngineService::PreFinishFrame()
{
	if (bChangeResolution && pNewWindowSize)
	{
		GameSettings::Instance()->SetWindowHeight(pNewWindowSize->height);
		LOG_I("Set Resolution to: %dx%d", pNewWindowSize->width,
			  pNewWindowSize->height);
		m_pRenderLoop->RecreateWindow(SFWindowRecreateData(std::move(*pNewWindowSize)));
		bChangeResolution = false;
	}

	if (bChangeWindowStyle)
	{
		GameSettings::Instance()->SetBorderless(newWindowStyle == SFWindowStyle::Bordlerless);
		m_pRenderLoop->RecreateWindow(SFWindowRecreateData(newWindowStyle));
		bChangeWindowStyle = false;
	}
}
} // namespace LittleEngine
