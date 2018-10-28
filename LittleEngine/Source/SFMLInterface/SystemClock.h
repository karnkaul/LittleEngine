#pragma once
#include "Fixed.h"
#include "SFML/System/Clock.hpp"

namespace LittleEngine {
	// \brief Real Time clock using sf::Clock
	// Note: this clock cannot be paused, only restarted
	namespace SystemClock {
		int GetCurrentMilliseconds();
		int64_t GetCurrentMicroseconds();
		void Restart();
	};
}
