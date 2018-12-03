#pragma once
#include <string>
#include "Engine/CoreGame.hpp"
#include "Engine/Logger/Logger.h"

#if ENABLED(LOG_PROFILING)
#define STOPWATCH_START(x) LittleEngine::Stopwatch::Start(x)
#define STOPWATCH_STOP() LittleEngine::Stopwatch::Stop()
#else
#define STOPWATCH_START(x)
#define STOPWATCH_STOP()
#endif

namespace LittleEngine {
	namespace Stopwatch {
		extern int g_logThresholdMS;
		extern Logger::Severity g_logSeverity;
		
		void Start(const std::string& name);
		void Stop();
	}
}
