#pragma once
#include "SFMLInterface/SystemClock.h"

namespace LittleEngine {
	class Object;

	// \brief Simple runtime stdout logger; 
	// Writes to debug.log using FileLogger
	namespace Logger {
		enum class Severity { Error, Warning, Info, Debug, HOT };
		
		void Log(const Object& context, const std::string& message, Severity severity = Severity::Info);
		void Log(const std::string& message, Severity severity = Severity::Info);
		void Log(const std::string& caller, const std::string& message, Severity severity = Severity::Info);
		void Cleanup();

		extern Severity g_logLevel;
	};
}
