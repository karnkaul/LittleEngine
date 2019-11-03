#include "SFMLAPI/Rendering/RenderStats.h"
#include "Core/BuildVersion.h"
#include "Core/Game/OS.h"
#include "Engine/FatalEngineException.h"
#include "Engine/Audio/LEAudio.h"
#include "Engine/Debug/Profiler.h"
#include "Engine/Debug/Tweakable.h"
#include "Engine/Context/LEContext.h"
#include "Engine/Rendering/LEShaders.h"
#include "Engine/Repository/LERepository.h"
#include "Game/Model/GameConfig.h"
#include "Game/Model/GameKernel.h"
#include "Game/Model/GameManager.h"
#include "Game/Model/World/WorldClock.h"
#include "Game/Model/World/WorldStateMachine.h"
#include "Game/Framework/Utility/Debug/Console/DebugConsole.h"
#include "Game/Framework/Utility/Debug/DebugProfiler.h"
#include "GameLoop.h"

namespace LittleEngine
{
using namespace Debug;

namespace
{
using namespace Core;
using namespace LittleEngine;

enum class PostRun
{
	Cleanup = 0,
	ReloadGame,
	ReloadApp,
	_COUNT
};

bool bTerminate = false;
PostRun postRun = PostRun::Cleanup;
Time maxFrameTime;

// App scope
std::unique_ptr<LERepository> uRepository;
std::unique_ptr<LEAudio> uAudio;

// Game scope
GameKernel kernel;

TweakS32(reload, nullptr);
TweakS32(ticksPerSec, nullptr);

bool Stage()
{
	if (GameLoop::g_createWorlds)
	{
		GameLoop::g_createWorlds();
	}
	if (!kernel.Boot())
	{
		return false;
	};
#if ENABLED(CONSOLE)
	Console::Init();
#endif
#if ENABLED(PROFILER)
	Profiler::Init(Time::Milliseconds(10));
#endif
#if defined(DEBUGGING)
	uAudio->InitDebug(*kernel.Context()->Renderer());
#endif
	postRun = PostRun::Cleanup;
	bTerminate = false;
	WorldClock::Reset();
	return true;
}

#if defined(DEBUGGING)
inline void ProfileFrameTime(Time frameStart, Time maxFrameTime)
{
	Time frameElapsed = Time::Now() - frameStart;
	static const Time DILATED_TIME = Time::Seconds(3);
	static Time logTime = Time::Now() - Time::Seconds(3);
	static u8 consecutive = 0;
	static constexpr u8 MIN_CONSECUTIVE = 3;
	if (frameElapsed > maxFrameTime)
	{
		if ((Time::Now() - logTime) > DILATED_TIME && consecutive >= MIN_CONSECUTIVE)
		{
			f32 max = maxFrameTime.AsSeconds() * 1000.0f;
			f32 taken = frameElapsed.AsSeconds() * 1000.0f;
			LOG_E("Game Loop is taking too long [%.2fms (max: %.2fms)]! Game time is inaccurate (slowed down)", taken, max);
			logTime = Time::Now();
			consecutive = 0;
		}
		++consecutive;
	}
	else
	{
		consecutive = 0;
	}
}
#endif

void Sleep(Time time)
{
	if (time.AsMilliseconds() > 0)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(time.AsMilliseconds()));
	}
}

void Unstage()
{
#if ENABLED(CONSOLE)
	Console::Cleanup();
#endif
#if ENABLED(PROFILER)
	Profiler::Cleanup();
#endif
#if defined(DEBUGGING)
	uAudio->DestroyDebug();
#endif
	kernel.Shutdown();
	LEShaders::Clear();
	uRepository->ResetState();
}

void Cleanup()
{
	uAudio = nullptr;
	uRepository = nullptr;
#if !defined(SHIPPING)
	GameConfig::Instance().Save(OS::Env()->FullPath(GameLoop::GAME_CONFIG_FILE.c_str()));
#endif
	Core::Jobs::Cleanup();
	LOG_I("[GameLoop] Terminated");
	OS::StopFileLogging();
}

void RunLoop(const Time tickRate, const Time fdt)
{
	Time accumulator;
	Time frameStart = Time::Now();
	kernel.Start(GameLoop::IDs::MAIN_MANIFEST, GameLoop::IDs::GAME_STYLE, GameLoop::g_loadShaders);
	while (!kernel.Context()->IsTerminating() && !bTerminate)
	{
		LEContext* pContext = kernel.Context();
		Time frameElapsed;
		pContext->PollInput();
		// Break and exit if Window closed
		if (pContext->IsTerminating())
		{
			break;
		}
		pContext->StartFrame();
		const Time now = Time::Now();
		const Time dt = Time::Clamp(now - frameStart, Time::Zero, maxFrameTime.Scaled(2));
		frameStart = now;
		WorldClock::Tick(dt);
#if ENABLED(RENDER_STATS)
		g_renderStats.dtList.push_front(dt);
		if (g_renderStats.dtList.size() > 16)
		{
			g_renderStats.dtList.pop_back();
		}
#endif

		// Step
		accumulator += dt;
		PROFILE_CUSTOM("STEP", maxFrameTime.Scaled(Fixed::OneHalf), Colour(127, 130, 255));
		while (accumulator >= fdt)
		{
			kernel.Step(fdt);
			accumulator -= fdt;
		}
		PROFILE_STOP("STEP");

		// Tick
		pContext->FireInput();
		pContext->Update();
		uRepository->Tick(dt);
		PROFILE_CUSTOM("TICK", maxFrameTime.Scaled(Fixed::OneHalf), Colour(127, 0, 255));
		kernel.Tick(dt);
		uAudio->Tick(dt);
		PROFILE_STOP("TICK");
#if ENABLED(CONSOLE)
		Console::Tick(dt);
#endif
#if ENABLED(PROFILER)
		Profiler::Tick(dt);
#endif
		// Submit
		pContext->SubmitFrame();
		if (pContext->IsTerminating())
		{
			break;
		}
#if defined(DEBUGGING)
		ProfileFrameTime(frameStart, maxFrameTime);
#endif
		frameElapsed = Time::Now() - frameStart;
		Sleep(tickRate - frameElapsed);
	}
}
} // namespace

namespace GameLoop
{
// Globals
Task g_createWorlds;
Task g_loadShaders;

namespace IDs
{
std::string COOKED_ASSETS = "GameAssets.cooked";
std::string MAIN_MANIFEST = "Manifest.amf";
std::string DEFAULT_FONT = "Fonts/Default.ttf";
std::string GAME_STYLE = "Texts/Game.style";
} // namespace IDs

std::string ASSETS_ROOT = "Resources/GameAssets";
std::string MUSIC_ROOT = "Resources/GameMusic";
std::string MAIN_MANIFEST_FILE = ASSETS_ROOT + "/" + IDs::MAIN_MANIFEST;

#if !defined(SHIPPING)
std::string GAME_CONFIG_FILE = ".game.conf";
#endif
} // namespace GameLoop

bool GameLoop::Init()
{
	auto& config = GameConfig::Instance();
	if (OS::Threads::VacantThreadCount() > 0)
	{
		std::string header = "Build: ";
		header += BUILD_VERSION;
		const u8 backupCount = config.BackupLogFileCount();
		OS::StartFileLogging(OS::Env()->FullPath("Debug"), backupCount, std::move(header));
	}
	try
	{
		uRepository = std::make_unique<LERepository>(IDs::DEFAULT_FONT, IDs::COOKED_ASSETS, ASSETS_ROOT);
		uAudio = std::make_unique<LEAudio>(MUSIC_ROOT);
		LEShaders::Init();
	}
	catch (const FatalEngineException& e)
	{
		LOG_E("[GameLoop] ERROR! Initialisation failed!\n%s", e.what());
		OS::StopFileLogging();
		return false;
	}

	Core::Jobs::Init(config.JobWorkerCount());
	maxFrameTime = config.MaxFrameTime();
#if ENABLED(TWEAKABLES)
	ticksPerSec.BindCallback([](std::string_view val) {
		s32 newRate = Strings::ToS32(std::string(val));
		if (newRate > 0 && newRate < 250)
		{
			Time newTickRate = Time::Seconds(1.0f / newRate);
			kernel.Context()->ModifyTickRate(newTickRate);
			LOG_I("[GameLoop] Tick Rate changed to %.2f ms", newTickRate.AsSeconds() * 1000.0f);
		}
		else
		{
			LOG_W("[GameLoop] Invalid value for ticks per second: %s", val.data());
		}
	});
	reload.m_value = "-1";
	reload.BindCallback([](std::string_view val) {
		s32 option = Strings::ToS32(std::string(val), -1);
		if (option >= 0 && option < ToS32(GameLoop::ReloadType::_COUNT))
		{
			GameLoop::Reload(static_cast<GameLoop::ReloadType>(option));
		}
		reload.m_value = "-1";
	});
#endif
	return true;
}

s32 GameLoop::Run()
{
	const GameConfig& config = GameConfig::Instance();
	while (true)
	{
		if (!Stage())
		{
			return 1;
		}
		RunLoop(config.TickRate(), config.StepRate());
		Unstage();
		if (postRun == PostRun::ReloadApp)
		{
			uRepository->UnloadAll(true);
			uRepository->LoadDefaultFont(IDs::DEFAULT_FONT);
		}
		else if (postRun == PostRun::Cleanup)
		{
			break;
		}
	}
	Cleanup();
	return 0;
}

bool GameLoop::Reload(ReloadType type)
{
	if (!kernel.Context())
	{
		LOG_E("[GameLoop] Cannot reload empty context!");
		return false;
	}
	switch (type)
	{
	default:
	case ReloadType::World:
		Assert(g_pGameManager, "GameManager is null!");
		g_pGameManager->ReloadWorld();
		break;
	case ReloadType::Game:
		postRun = PostRun::ReloadGame;
		bTerminate = true;
		break;
	case ReloadType::App:
		postRun = PostRun::ReloadApp;
		bTerminate = true;
		break;
	}
	return true;
}
} // namespace LittleEngine
