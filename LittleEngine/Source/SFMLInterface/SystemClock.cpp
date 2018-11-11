#include "le_stdafx.h"
#include "SystemClock.h"

namespace LittleEngine {
	namespace SystemClock {
		namespace Local {
			sf::Clock clock;
		}

		using namespace Local;

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
