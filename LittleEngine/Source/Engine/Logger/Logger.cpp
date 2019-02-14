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
		void Cout(const std::string& log);

#pragma endregion
		namespace {
#pragma region Definitions
			std::unique_ptr<FileLogger> fileLogger = std::make_unique<FileLogger>("debug.log");
			struct sudo {};
#pragma endregion

#pragma region Internal
			void Log(const std::string& severity, const std::string& caller, const std::string& message) {
				std::string suffix = (caller.length() > 0) ? " [" + caller + "]" : "";
				suffix += (" [" + GameClock::ToString(SystemClock::GetCurrentMilliseconds()) + "]");
				std::string log = severity + " " + message + suffix;
				Cout(log);
				if (fileLogger) fileLogger->AddToBuffer(std::move(log));
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
		}
#pragma endregion

#pragma region Implmementation
		// FileLogger extern access
		void Cout(const std::string& message) {
#if DEBUG
			std::cout << message << std::endl;
#endif
#if defined(_WIN32)
			OutputDebugString((message + "\n").c_str());
#endif
		}

		void Log(const Object& context, const std::string & message, Severity severity) {
			Log(context.GetName(), message, severity);
		}

		void Log(const std::string& message, Severity severity) {
			Log("", message, severity);
		}

		void Log(std::string caller, const std::string& message, Severity severity) {
			if (severity <= g_logLevel) Log(SeverityString(severity), caller, message);
		}

		void Cleanup() {
			fileLogger = nullptr;
		}
	}
#pragma endregion
}
