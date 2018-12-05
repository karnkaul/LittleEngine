#pragma once
#include "Fixed.h"
#include "SFMLInterface/SystemClock.h"

namespace LittleEngine {
	using Fixed = GameUtils::Fixed;

	// \brief Use to measure game time
	// Note: This DOES NOT run on its own, requires Engine to call Tick(deltaTime)!
	class GameClock {
	private:
		// Base game time maintained by Engine
		static double gameTime;
		// An instance will use this offset to calculate its current time
		double m_startTime;

	public:
		GameClock();

		void Restart();
		int64_t GetElapsedMicroSeconds() const;
		int GetElapsedMilliSeconds() const;
		
		static int GetGameTimeMilliSeconds();
		static std::string ToString(int milliseconds);

	private:
		// Global Ticks to be provided by Engine
		static void Tick(const Fixed& deltaTime);
		// On App Reload etc
		static void Reset();

		friend class Engine;
	};
}
