#include "le_stdafx.h"
#include <iostream>
#include "Logger.h"
#include "FileLogger.h"
#include "Engine/Object.h"
#include "Engine/GameClock.h"
#if defined (_WIN32)
#include <Windows.h>
#endif

namespace LittleEngine {
	namespace Logger {
#pragma region Globals
		Severity g_logLevel = Severity::Info;
#pragma endregion
		namespace Local {
#pragma region Definitions
			std::unique_ptr<FileLogger> fileLogger = std::make_unique<FileLogger>("debug.log");
			struct sudo {};
#pragma endregion

#pragma region Internal
			void Cout(const std::string& severity, const std::string& caller, const std::string& message) {
				std::string suffix = (caller.length() > 0) ? " [" + caller + "]" : "";
				suffix += (" [" + GameClock::ToString(SystemClock::GetCurrentMilliseconds()) + "]");
				std::string log = severity + " " + message + suffix;
				if (fileLogger) {
					fileLogger->AddToBuffer(log);
				}
#if DEBUG
				std::cout << log << std::endl;
#endif
#if defined(_WIN32)
				OutputDebugString((log + "\n").c_str());
#endif
			}

			std::string SeverityString(Logger::Severity severity) {
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

			void LoggerLog(const std::string& message, Logger::Severity severity = Logger::Severity::Info) {
				Cout(SeverityString(severity), "Logger", message);
			}
		}
#pragma endregion

#pragma region Implmementation
		using namespace Local;

		void Log(const Object& context, const std::string & message, Severity severity) {
			Log(context.GetName(), message, severity);
		}

		void Log(const std::string& message, Severity severity) {
			Log("", message, severity);
		}

		void Log(std::string caller, const std::string& message, Severity severity) {
			if (severity <= g_logLevel) {
				Cout(SeverityString(severity), caller, message);
			}
		}

		void Cleanup() {
			fileLogger = nullptr;
		}
	}
#pragma endregion
}
