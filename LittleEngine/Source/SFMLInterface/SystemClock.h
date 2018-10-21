#pragma once
#include "Fixed.h"
#include "SFML/System/Clock.hpp"

namespace LittleEngine {
	// \brief Real Time clock using sf::Clock
	// Note: this clock cannot be paused, only restarted
	class SystemClock {
	public:
		int GetCurrentMilliseconds() const;
		int64_t GetCurrentMicroseconds() const;
		void Restart();
	private:
		sf::Clock clock;
	};
}
