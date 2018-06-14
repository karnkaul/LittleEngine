#include "stdafx.h"
#include "SystemClock.h"

namespace Game {
	int SystemClock::GetCurrentMilliseconds() const {
		return clock.getElapsedTime().asMilliseconds();
	}

	int64_t SystemClock::GetCurrentMicroseconds() const {
		return static_cast<int64_t>(clock.getElapsedTime().asMicroseconds());
	}

	void SystemClock::Restart() {
		clock.restart();
	}
}
