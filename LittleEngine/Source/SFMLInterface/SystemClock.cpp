#include "le_stdafx.h"
#include "SystemClock.h"

namespace LittleEngine {
	namespace SystemClock {
		static sf::Clock s_clock;

		int GetCurrentMilliseconds() {
			return s_clock.getElapsedTime().asMilliseconds();
		}

		int64_t GetCurrentMicroseconds() {
			return static_cast<int64_t>(s_clock.getElapsedTime().asMicroseconds());
		}

		void Restart() {
			s_clock.restart();
		}
	}
}
