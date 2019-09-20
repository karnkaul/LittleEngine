#include "Core/Logger.h"
#include "Core/Version.h"
#include "Engine/FatalEngineException.h"
#include "Engine/Debug/Profiler.h"
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
UPtr<GameManager> uGM;

// Game Loop
Time maxFrameTime;
bool bPauseOnFocusLoss = false;

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
			Time newTickRate = Time::Seconds(1.0f / newRate);
			uGM->ModifyTickRate(newTickRate);
			LOG_I("[GameLoop] Tick Rate changed to %.2f ms", newTickRate.AsSeconds() * 1000.0f);
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
	ControllerComponent::s_orientationEpsilon = config.ControllerOrientationEpsilon();
#if defined(DEBUGGING)
	ControllerComponent::s_orientationWidthHeight = config.ControllerOrientationSize();
	Entity::s_orientationWidthHeight = config.EntityOrientationSize();
#endif

	if (config.ShouldCreateRenderThread())
	{
		if (OS::Threads::VacantThreadCount() == 0)
		{
			LOG_W("[GameLoop] Insufficient threads to create render thread!\n!ERROR! Async Renderer "
				  "not "
				  "available!");
			config.m_bRenderThread = false;
		}
	}

	Core::Jobs::Init(config.JobWorkerCount());

#if defined(DEBUGGING)
	Collider::s_debugShapeWidth = config.ColliderBorderWidth();
#endif
	maxFrameTime = config.MaxFrameTime();

	uShaders = MakeUnique<LEShaders>();

	Locale::Init(pSettings->LocdataID(), pSettings->ENLocdataID());
	Time::Reset();

	return true;
}

bool Tick(Time dt)
{
	PROFILE_START("TICK", Colour(127, 0, 255));
	uRepository->Tick(dt);
	bool bYield = false;
	uGM->Tick(dt, bYield);
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
	uGM = nullptr;
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

	uGM = MakeUnique<GameManager>();
	uGM->CreateContext(config);
#if ENABLED(CONSOLE)
	Console::Init();
#endif
#if ENABLED(PROFILER)
	Profiler::Init(Time::Milliseconds(10));
#endif
	WorldClock::Reset();
	uGM->Start("Manifest.amf", "Texts/Game.style", &GameInit::LoadShaders);
		
	const Time tickRate = config.TickRate();
	Time accumulator;
	Time currentTime = Time::Now();
	LEContext* pContext = uGM->Context();
	while (!pContext->IsTerminating())
	{
		Time frameElapsed;
		pContext->PollInput();
		// Break and exit if Window closed
		if (pContext->IsTerminating())
		{
			break;
		}

		if (!pContext->IsPaused())
		{
			pContext->StartFrame();
			const Time dt = tickRate;
			const Time newTime = Time::Now();
			const Time frameTime = Time::Clamp(newTime - currentTime, Time::Zero, maxFrameTime);
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

			pContext->SubmitFrame();

			if (pContext->IsTerminating())
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
			frameElapsed = tickRate.Scaled(Fixed(0.25f));
		}
		Sleep(tickRate - frameElapsed);
	}

	Cleanup();
	return 0;
}
} // namespace LittleEngine
