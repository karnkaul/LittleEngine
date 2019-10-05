#include "Core/Logger.h"
#include "Core/Version.h"
#include "SFMLAPI/Rendering/RenderStats.h"
#include "Engine/FatalEngineException.h"
#include "Engine/Debug/Profiler.h"
#include "Model/GameConfig.h"
#include "Model/GameKernel.h"
#include "Model/World/WorldClock.h"
#include "Framework/Framework.h"
#include "Framework/Utility/Debug/Console/DebugConsole.h"
#include "Framework/Utility/Debug/DebugProfiler.h"
#include "Gameplay/GameInit.h"
#include "GameLoop.h"

namespace LittleEngine
{
using namespace Debug;
extern Fixed g_maxParticlesScale;

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

// Globals
namespace IDs
{
const std::string ASSETS_ROOT = "GameAssets";
const std::string COOKED_ASSETS = "GameAssets.cooked";
const std::string MAIN_MANIFEST = "Manifest.amf";
const std::string DEFAULT_FONT = "Fonts/Default.ttf";
const std::string GAME_STYLE = "Texts/Game.style";
} // namespace IDs

const std::string MAIN_MANIFEST_FILE = "GameAssets/Manifest.amf";
#if !defined(SHIPPING)
const std::string GAME_CONFIG_FILE = ".game.conf";
#endif

bool bInit = false;
bool bTerminate = false;
PostRun postRun = PostRun::Cleanup;
Time maxFrameTime;
GameConfig config;

// App scope
UPtr<LERepository> uRepository;
UPtr<LEAudio> uAudio;

// Game scope
GameKernel kernel;

TweakS32(reload, nullptr);
TweakS32(ticksPerSec, nullptr);

bool Init(s32 argc, char** argv)
{
	OS::Env()->SetVars(argc, argv, {IDs::COOKED_ASSETS.c_str(), MAIN_MANIFEST_FILE.c_str()});
	config.Init();
#if !defined(SHIPPING)
	LOG_D("[GameLoop] Loading config...");
	config.Load(OS::Env()->FullPath(GAME_CONFIG_FILE.c_str()));
#endif
	if (OS::Threads::VacantThreadCount() > 0)
	{
		std::string header = "Build: ";
		header += Core::Version::BUILD_VERSION.ToString();
		u8 backupCount = config.BackupLogFileCount();
		Core::StartFileLogging(OS::Env()->FullPath("Debug"), backupCount, std::move(header));
	}
	try
	{
		uRepository = MakeUnique<LERepository>(IDs::DEFAULT_FONT, IDs::COOKED_ASSETS, IDs::ASSETS_ROOT);
		uAudio = MakeUnique<LEAudio>();
		LEShaders::Init();
	}
	catch (const FatalEngineException& e)
	{
		LOG_E("[GameLoop] ERROR! Initialisation failed!\n%s", e.what());
		return false;
	}
	auto pSettings = GameSettings::Instance();
	f32 maxParticlesScale = 1.0f;
	auto pStr = pSettings->GetValue("LOW_QUALITY");
	if (pStr && Strings::ToBool(*pStr))
	{
		maxParticlesScale = 0.25f;
	}
	pStr = pSettings->GetValue("MAX_PARTICLES_SCALE");
	if (pStr)
	{
		maxParticlesScale = Strings::ToF32(*pStr, maxParticlesScale);
	}
	g_maxParticlesScale = Fixed(maxParticlesScale);
	Core::g_MinLogSeverity = pSettings->LogLevel();
	ControllerComponent::s_orientationEpsilon = config.ControllerOrientationEpsilon();
	maxFrameTime = config.MaxFrameTime();
	if (config.ShouldCreateRenderThread())
	{
		if (OS::Threads::VacantThreadCount() == 0)
		{
			LOG_W("[GameLoop] Insufficient threads to create render thread!\n!ERROR! Async Renderer not available!");
			config.m_bRenderThread = false;
		}
	}
#if defined(DEBUGGING)
	ControllerComponent::s_orientationWidthHeight = config.ControllerOrientationSize();
	Entity::s_orientationWidthHeight = config.EntityOrientationSize();
	Collider::s_debugShapeWidth = config.ColliderBorderWidth();
#endif
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
	Core::Jobs::Init(config.JobWorkerCount());
	Locale::Init(pSettings->LocdataID(), pSettings->ENLocdataID());
	Time::Reset();
	return true;
}

bool Stage()
{
	GameInit::CreateWorlds();
	if (!kernel.Boot(config)) 
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
	config.Save(OS::Env()->FullPath(GAME_CONFIG_FILE.c_str()));
#endif
	Core::Jobs::Cleanup();
	LOG_I("[GameLoop] Terminated");
	Core::StopFileLogging();
}

void RunLoop(const Time tickRate, const Time fdt)
{
	Time accumulator;
	Time frameStart = Time::Now();
	kernel.Start(IDs::MAIN_MANIFEST, IDs::GAME_STYLE, &GameInit::LoadShaders);
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

s32 GameLoop::Run(s32 argc, char** argv)
{
	if (!bInit)
	{
		if (!(bInit = Init(argc, argv)))
		{
			LOG_E("[GameLoop] Fatal Error initialising GameLoop!");
			Cleanup();
			return 1;
		}
	}

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
	if (!bInit)
	{
		LOG_E("[GameLoop] Cannot reload context without initialisation!");
		return false;
	}
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
