#include "le_stdafx.h"
#include "Stopwatch.h"
#include <string>
#include "SFMLInterface/SystemClock.h"
#include "Utils.h"

namespace LittleEngine {
	namespace Stopwatch {
		int g_logThresholdMS = 50;
		Logger::Severity g_logSeverity = Logger::Severity::Warning;

		static bool s_bRunning = false;
		static std::string s_name;
		static std::string s_lapName;
		static int s_startMS = 0;
		static int s_endMS = 0;

		static void Stop(bool bInterrupted) {
			s_endMS = SystemClock::GetCurrentMilliseconds();
			int elapsed = s_endMS - s_startMS;
			if (elapsed >= g_logThresholdMS) {
				std::string prefix = bInterrupted ? "INTERRUPTED: " : "STOPWATCH: ";
				Logger::Log(prefix + s_lapName + " took (" + Strings::ToString(elapsed) + " ms)", g_logSeverity);
			}
		}

		void Start(const std::string& lapName) {
			if (s_bRunning) {
				Stop(true);
			}
			s_lapName = lapName;
			s_startMS = SystemClock::GetCurrentMilliseconds();
		}

		void Stop() {
			Stop(false);
		}
	};
}
