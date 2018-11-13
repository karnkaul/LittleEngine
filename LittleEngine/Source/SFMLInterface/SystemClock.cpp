#include "le_stdafx.h"
#include "SystemClock.h"

namespace LittleEngine {
	namespace SystemClock {
		namespace {
			sf::Clock clock;
		}

		int GetCurrentMilliseconds() {
			return clock.getElapsedTime().asMilliseconds();
		}

		int64_t GetCurrentMicroseconds() {
			return static_cast<int64_t>(clock.getElapsedTime().asMicroseconds());
		}

		void Restart() {
			clock.restart();
		}
	}
}
