#pragma once
#include "Core/CoreTypes.h"

namespace LittleEngine
{
// \brief Use to measure game time
// Note: This DOES NOT run on its own, requires SFEventLoop to call Tick()!
class GameClock final
{
private:
	// Base game time maintained by SFEventLoop
	static Time gameTime;
	// An instance will use this offset to calculate its current time
	Time m_startTime;

public:
	static Time GetGameTime();
	static String ToString(Time time);

	GameClock();
	~GameClock();

	void Restart();
	Time GetElapsed() const;

private:
	// Global Ticks to be provided by SFEventLoop
	static void Tick(Time dt);
	// On App Reload etc
	static void Reset();

	friend class ASFEventLoop;
};
} // namespace LittleEngine
