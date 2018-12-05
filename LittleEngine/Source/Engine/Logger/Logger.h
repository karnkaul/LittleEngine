#pragma once
#include "SFMLInterface/SystemClock.h"
#include "Engine/CoreGame.hpp"

namespace LittleEngine {
	class Object;

#if GAME_DEBUG
	#define DEBUG_ASSERT(expr, errLog)		if (!(expr)) { Log(errLog, Logger::Severity::Error); }
#else
	#define DEBUG_ASSERT(x, y)
#endif

	// \brief Simple runtime stdout logger; 
	// Writes to debug.log using FileLogger
	namespace Logger {
		enum class Severity { Error, Warning, Info, Debug, HOT };
		
		void Log(const Object& context, const std::string& message, Severity severity = Severity::Info);
		void Log(const std::string& message, Severity severity = Severity::Info);
		void Log(std::string caller, const std::string& message, Severity severity = Severity::Info);
		void Cleanup();

		extern Severity g_logLevel;
	};
}
