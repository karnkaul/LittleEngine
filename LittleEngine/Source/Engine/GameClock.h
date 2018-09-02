#pragma once
#include "Utils/Fixed.h"
#include "SFMLInterface/SystemClock.h"

namespace LittleEngine {
	using Fixed = Utils::Fixed;

	// \brief Use to measure game time
	// Note: This DOES NOT run on its own, requires Engine to call Tick(deltaTime)!
	class GameClock {
	public:
		GameClock();

		int64_t GetElapsedMicroSeconds() const;
		int GetElapsedMilliSeconds() const;
		
		static int GetGameTimeMilliSeconds();
		static std::string ToString(int milliseconds);

	private:
		friend class Engine;
		// Global Ticks to be provided by Engine
		static void Tick(const Fixed& deltaTime);
		// On App Reload etc
		static void Reset();

		// Base game time maintained by Engine
		static double gameTime;
		// An instance will use this offset to calculate its current time
		double startTime;
	};
}
