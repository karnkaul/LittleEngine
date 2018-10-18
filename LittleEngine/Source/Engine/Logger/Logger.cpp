#include "le_stdafx.h"
#include <iostream>
#include "Logger.h"
#include "FileLogger.h"
#include "Engine/Object.h"
#include "Engine/GameClock.h"

namespace LittleEngine {
	namespace Logger {
#pragma region Globals
		Severity g_logLevel = Severity::Info;
#pragma endregion

#pragma region Definitions
		static std::unique_ptr<FileLogger> s_fileLogger = std::make_unique<FileLogger>("debug.log");
		static std::unique_ptr<SystemClock> clock = std::make_unique<SystemClock>();
		struct sudo {};
#pragma endregion

#pragma region Internal
		static void Cout(const std::string& severity, const std::string& caller, const std::string& message) {
			std::string suffix = (caller.length() > 0) ? " [" + caller + "]" : "";
			suffix += (" [" + GameClock::ToString(clock->GetCurrentMilliseconds()) + "]");
			std::string log = severity + " " + message + suffix;
			if (s_fileLogger) {
				s_fileLogger->AddToBuffer(log);
			}
			std::cout << log << std::endl;
		}

		static std::string SeverityString(Logger::Severity severity) {
			char prefix = '[';
			char suffix = ']';
			std::string value = "I";
			switch (severity) {
			case Logger::Severity::Error:
				value = "E";
				break;
			case Logger::Severity::Warning:
				value = "W";
				break;
			case Logger::Severity::Info:
				value = "I";
				break;
			case Logger::Severity::Debug:
				value = "D";
				break;
			case Logger::Severity::HOT:
				value = "H";
				break;
			}
			return prefix + value + suffix;
		}

		static void LoggerLog(const std::string& message, Logger::Severity severity = Logger::Severity::Info) {
			Cout(SeverityString(severity), "Logger", message);
		}
#pragma endregion

#pragma region Implmementation
		void Log(const Object& context, const std::string & message, Severity severity) {
			Log(context.GetName(), message, severity);
		}

		void Log(const std::string& message, Severity severity) {
			Log("", message, severity);
		}

		void Log(const std::string& caller, const std::string& message, Severity severity) {
#if !DEBUG
			return;
#endif
			if (severity <= g_logLevel) {
				Cout(SeverityString(severity), caller, message);
			}
		}

		void Cleanup() {
			s_fileLogger = nullptr;
		}
	}
#pragma endregion
}
