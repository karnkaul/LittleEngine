#pragma once
#include <string>
#include "SFMLInterface/SystemClock.h"
#include "Engine/Logger/Logger.h"

namespace LittleEngine {
	struct Stopwatch {
		int logThresholdMS = 30;
		Logger::Severity logSeverity = Logger::Severity::Warning;

		Stopwatch(const std::string& name = "") : name("STOPWATCH " + name + ": ") {}

		void Start(const std::string& lapName) {
			this->lapName = lapName;
			startMS = SystemClock::GetCurrentMilliseconds();
		}

		void Stop() {
			endMS = SystemClock::GetCurrentMilliseconds();
			int elapsed = endMS - startMS;
			if (elapsed >= logThresholdMS) {
				Logger::Log(name + lapName + " took (" + std::to_string(elapsed) + " ms)", logSeverity);
			}
		}

	private:
		std::string name;
		std::string lapName;
		int startMS = 0;
		int endMS = 0;
	};
}
