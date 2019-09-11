#include "Core/Logger.h"
#include "Core/Version.h"
#include "Engine/FatalEngineException.h"
#include "Engine/Debug/Profiler.h"
#include "Engine/Locale/Locale.h"
#include "Model/GameConfig.h"
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

// Globals
UPtr<LERepository> uRepository;
UPtr<LEShaders> uShaders;
UPtr<LEAudio> uAudio;

// LEContext
GameConfig config;
UPtr<LEContext> uContext;
UPtr<WorldStateMachine> uWSM;

// Game Loop
Time tickRate;
Time maxFrameTime;
bool bPauseOnFocusLoss = false;
bool bRenderThread = true;

TweakS32(ticksPerSec, nullptr);

bool Init(s32 argc, char** argv)
{
	OS::Env()->SetVars(argc, argv, {"GameAssets.cooked", "GameAssets/Manifest.amf"});

	config.Init();
#if !defined(SHIPPING)
	LOG_D("[GameLoop] Initialising event loop, loading config...");
	config.Load(OS::Env()->FullPath(".game.conf"));
#endif
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

	if (OS::Threads::VacantThreadCount() > 0)
	{
		String header = "Build: " + Core::Version::BUILD_VERSION.ToString();
		u8 backupCount = config.BackupLogFileCount();
		Core::StartFileLogging(OS::Env()->FullPath("Debug"), backupCount, std::move(header));
	}

#if ENABLED(TWEAKABLES)
	ticksPerSec.BindCallback([](const String& val) {
		s32 newRate = Strings::ToS32(val);
		if (newRate > 0 && newRate < 250)
		{
			tickRate = Time::Seconds(1.0f / newRate);
			uContext->ModifyTickRate(tickRate);
			LOG_I("[GameLoop] Tick Rate changed to %.2f ms", tickRate.AsSeconds() * 1000.0f);
		}
		else
		{
			LOG_W("[GameLoop] Invalid value for ticks per second: %s", val.c_str());
		}
	});
#endif
	try
	{
		uRepository = MakeUnique<LERepository>("Fonts/main.ttf", "GameAssets.cooked", "GameAssets");
		uAudio = MakeUnique<LEAudio>();
	}
	catch (const FatalEngineException& /*e*/)
	{
		LOG_E("[GameLoop] ERROR! Could not initialise Engine Service!");
		return false;
	}

	Core::g_MinLogSeverity = pSettings->LogLevel();
	bPauseOnFocusLoss = config.ShouldPauseOnFocusLoss();
	bRenderThread = config.ShouldCreateRenderThread();
	ControllerComponent::s_orientationEpsilon = config.ControllerOrientationEpsilon();
#if defined(DEBUGGING)
	ControllerComponent::s_orientationWidthHeight = config.ControllerOrientationSize();
	Entity::s_orientationWidthHeight = config.EntityOrientationSize();
#endif

	if (bRenderThread)
	{
		if (OS::Threads::VacantThreadCount() == 0)
		{
			LOG_W("[GameLoop] Insufficient threads to create render thread!\n!ERROR! Async Renderer "
				  "not "
				  "available!");
			bRenderThread = false;
		}
	}

	Core::Jobs::Init(config.JobWorkerCount());

#if defined(DEBUGGING)
	Collider::s_debugShapeWidth = config.ColliderBorderWidth();
#endif
	tickRate = config.TickRate();
	maxFrameTime = config.MaxFrameTime();

	uShaders = MakeUnique<LEShaders>();

	Locale::Init(pSettings->LocdataID(), pSettings->ENLocdataID());
	Time::Reset();

	return true;
}

void CreateContext(GameConfig& config)
{
	GameSettings& settings = *GameSettings::Instance();
	LEContextData data;
	Vector2 viewSize = config.ViewSize();
	data.viewportData.viewportSize = settings.SafeGetViewportSize(viewSize);
	data.viewportData.viewSize = viewSize;
	data.viewportData.title = LOC(config.TitleBarText());
	data.viewportData.style = settings.GetViewportStyle();
	data.tickRate = tickRate;
	data.bRenderThread = bRenderThread;
	data.renderThreadStartDelay = config.RenderThreadStartDelay();
	data.bPauseOnFocusLoss = config.ShouldPauseOnFocusLoss();
	Core::Property::Persistor inputMapPersistor;
	auto pInputMapFile = settings.GetValue("CUSTOM_INPUT_MAP");
	if (pInputMapFile)
	{
		String inputMapFile = OS::Env()->FullPath(pInputMapFile->c_str());
		if (inputMapPersistor.Load(inputMapFile))
		{
			u16 count = data.inputMap.Import(inputMapPersistor);
			if (count > 0)
			{
				LOG_I("[GameLoop] Loaded %u custom Input Mappings successfully", count);
			}
		}
	}
	uContext = MakeUnique<LEContext>(std::move(data));
}

bool Tick(Time dt)
{
	PROFILE_START("TICK", Colour(127, 0, 255));
	uRepository->Tick(dt);
	bool bYield = false;
	uWSM->Tick(dt, bYield);
	bYield |= uContext->Update();
	uAudio->Tick(dt);
#if ENABLED(CONSOLE)
	Console::Tick(dt);
#endif
	PROFILE_STOP("TICK");
#if ENABLED(PROFILER)
	Profiler::Tick(dt);
#endif
	return bYield;
}

#if defined(DEBUGGING)
inline void ProfileFrameTime(Time frameElapsed, Time maxFrameTime)
{
	static const Time DILATED_TIME = Time::Milliseconds(250);
	static const u8 MAX_CONSECUTIVE = 2;
	static Time logTime = Time::Now() - Time::Milliseconds(300);
	static u8 consecutive = 0;
	if (frameElapsed > maxFrameTime)
	{
		++consecutive;
		if ((Time::Now() - logTime) > DILATED_TIME && consecutive > MAX_CONSECUTIVE)
		{
			f32 max = maxFrameTime.AsSeconds() * 1000.0f;
			f32 taken = frameElapsed.AsSeconds() * 1000.0f;
			LOG_E("Game Loop is taking too long! Game time is inaccurate (slowed down) "
				  "[max: "
				  "%.2fms taken: %.2fms]",
				  max, taken);
			logTime = Time::Now();
			consecutive = 0;
		}
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

void Cleanup()
{
#if ENABLED(CONSOLE)
	Console::Cleanup();
#endif
#if ENABLED(PROFILER)
	Profiler::Cleanup();
#endif
	uWSM = nullptr;
	uContext = nullptr;
	uAudio = nullptr;
	uShaders = nullptr;
	uRepository = nullptr;
#if !defined(SHIPPING)
	config.Save(OS::Env()->FullPath(".game.conf"));
#endif
	Core::Jobs::Cleanup();
	LOG_I("[GameLoop] Terminated");
	Core::StopFileLogging();
}
} // namespace

s32 GameLoop::Run(s32 argc, char** argv)
{
	if (!Init(argc, argv))
	{
		LOG_E("[GameLoop] Fatal Error initialising GameLoop!");
		Cleanup();
		return -1;
	}

	CreateContext(config);
	uWSM = MakeUnique<WorldStateMachine>(*uContext);
#if ENABLED(CONSOLE)
	Console::Init(*uContext);
#endif
#if ENABLED(PROFILER)
	Profiler::Init(*uContext, Time::Milliseconds(10));
#endif
	WorldClock::Reset();
	uWSM->Start("Manifest.amf", "Texts/Game.style", &GameInit::LoadShaders);

	Time accumulator;
	Time currentTime = Time::Now();
	while (!uContext->IsTerminating())
	{
		Time frameElapsed;
		uContext->PollInput();
		// Break and exit if Window closed
		if (uContext->IsTerminating())
		{
			break;
		}

		if (!uContext->IsPaused())
		{
			uContext->StartFrame();
			Time dt = tickRate;
			Time newTime = Time::Now();
			Time frameTime = Time::Clamp(newTime - currentTime, Time::Zero, maxFrameTime);
			currentTime = newTime;

			accumulator += frameTime;
			while (accumulator >= dt)
			{
				WorldClock::Tick(dt);
				bool bYield = Tick(dt);
#if ENABLED(CONSOLE)
				Debug::Console::Tick(dt);
#endif
				if (bYield)
				{
					accumulator = Time::Zero;
					LOG_D("[EventLoop] Yielded integration");
					break;
				}
				accumulator -= dt;
			}

			uContext->SubmitFrame();

			if (uContext->IsTerminating())
			{
				break;
			}
#if defined(DEBUGGING)
			ProfileFrameTime(Time::Now() - currentTime, tickRate);
#endif
			frameElapsed = Time::Now() - currentTime;
		}
		else
		{
			frameElapsed = tickRate.Scale(Fixed(0.25f));
		}
		Sleep(tickRate - frameElapsed);
	}

	Cleanup();
	return 0;
}
} // namespace LittleEngine
