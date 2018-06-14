#pragma once
#include "Utils/Fixed.h"
#include "SFML/System/Clock.hpp"

namespace Game {
	class SystemClock {
	public:
		int GetCurrentMilliseconds() const;
		int64_t GetCurrentMicroseconds() const;
		void Restart();
	private:
		sf::Clock clock;
	};
}
