#pragma once
#include "SFMLInterface/SystemClock.h"

namespace LittleEngine {
	class Object;
	class FileLogger;

	// \brief Simple runtime stdout logger; 
	// Writes to debug.log using FileLogger
	class Logger {
	public:
		enum class Severity { Error, Warning, Info, Debug, HOT };
		static void SetLogLevel(Severity logLevel);
		
		static void Log(const Object& context, const std::string& message, Severity severity = Severity::Info);
		static void Log(const std::string& message, Severity severity = Severity::Info);
		static void Log(const std::string& caller, const std::string& message, Severity severity = Severity::Info);
	private:
		std::unique_ptr<SystemClock> clock;
		static std::unique_ptr<FileLogger> fileLogger;
		static Severity logLevel;
		
		Logger();
		static Logger& Instance();
		struct sudo {};
		static void Log(const sudo& sudo, const std::string& message, Severity severity = Severity::Info);
		void Cout(const std::string& severity, const std::string& caller, const std::string& message);
	};
}
