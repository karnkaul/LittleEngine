#include "stdafx.h"
#include "EngineService.h"
#include "AsyncFileLogger.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/Engine/EngineLoop.h"
#include "LittleEngine/Engine/OS.h"
#include "SFMLAPI/Input/SFInputStateMachine.h"
#include "LittleEngine/Debug/DebugProfiler.h"
#include "LittleEngine/Debug/Console/DebugConsole.h"

namespace LittleEngine
{
#if DEBUGGING
using namespace Debug;
#endif
EngineService::EngineService()
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
		m_uFileLogger = MakeUnique<AsyncFileLogger>("debug.log");
	}
	m_uEngineInput = MakeUnique<EngineInput>();
	m_uAssetRepository = MakeUnique<EngineRepository>("Assets");
	m_uWorldStateMachine = MakeUnique<WorldStateMachine>();
	m_uEngineAudio = MakeUnique<EngineAudio>();
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
	m_uAssetRepository = nullptr;
	m_uEngineAudio = nullptr;
	Services::UnprovideEngine(*this);
	m_uFileLogger = nullptr;
}

WorldStateMachine* EngineService::Worlds() const
{
	return m_uWorldStateMachine.get();
}

EngineRepository* EngineService::Repository() const
{
	return m_uAssetRepository.get();
}

EngineInput* EngineService::Input() const
{
	return m_uEngineInput.get();
}

EngineAudio* EngineService::Audio() const
{
	return m_uEngineAudio.get();
}

void EngineService::Terminate()
{
	m_bTerminate = true;
}

void EngineService::PreRun()
{
	m_bTerminate = false;
#if ENABLED(CONSOLE)
	Console::Init();
#endif
#if ENABLED(PROFILER)
	Profiler::Init(std::this_thread::get_id());
#endif
	m_uWorldStateMachine->Start("GameAssets.amf");
}

void EngineService::UpdateInput(const SFInputDataFrame& inputDataFrame)
{
	m_uEngineInput->TakeSnapshot(inputDataFrame);
	m_uEngineInput->FireCallbacks();
}

void EngineService::Tick(Time dt)
{
	PROFILE_START("Engine::Tick", Colour::Magenta);
	Services::Jobs()->Tick(dt);
	m_uAssetRepository->Tick(dt);
	m_uWorldStateMachine->Tick(dt);
	m_uEngineAudio->Tick(dt);
#if ENABLED(CONSOLE)
	Console::Tick(dt);
#endif
	PROFILE_STOP("Engine::Tick");
#if ENABLED(PROFILER)
	Profiler::Tick(dt);
#endif
}

void EngineService::PostTick()
{
#if ENABLED(PROFILER)
	Profiler::Render();
#endif
}

void EngineService::PostBufferSwap()
{
	m_uWorldStateMachine->PostBufferSwap();
}
} // namespace LittleEngine
