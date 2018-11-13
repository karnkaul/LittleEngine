#include "le_stdafx.h"
#include "Stopwatch.h"
#include <string>
#include "SFMLInterface/SystemClock.h"
#include "Utils.h"

namespace LittleEngine {
	namespace Stopwatch {
		int g_logThresholdMS = 50;
		Logger::Severity g_logSeverity = Logger::Severity::Warning;

		namespace {
			bool bRunning = false;
			std::string name;
			std::string lapName;
			int startMS = 0;
			int endMS = 0;

			static void Stop(bool bInterrupted) {
				endMS = SystemClock::GetCurrentMilliseconds();
				int elapsed = endMS - startMS;
				if (elapsed >= g_logThresholdMS) {
					std::string prefix = bInterrupted ? "INTERRUPTED: " : "STOPWATCH: ";
					Logger::Log(prefix + lapName + " took (" + Strings::ToString(elapsed) + " ms)", g_logSeverity);
				}
			}
		}

		void Start(const std::string& _lapName) {
			if (bRunning) {
				Stop(true);
			}
			lapName = _lapName;
			startMS = SystemClock::GetCurrentMilliseconds();
		}

		void Stop() {
			Stop(false);
		}
	};
}
