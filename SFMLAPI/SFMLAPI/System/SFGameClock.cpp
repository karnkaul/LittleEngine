#include "stdafx.h"
#include "SFGameClock.h"
#include "Core/Utils.h"

namespace LittleEngine
{
Time GameClock::gameTime = Time::Zero;

GameClock::GameClock()
{
	m_startTime = gameTime;
}

GameClock::~GameClock() = default;

void GameClock::Restart()
{
	m_startTime = gameTime;
}

Time GameClock::GetElapsed() const
{
	return gameTime - m_startTime;
}

Time GameClock::GetGameTime()
{
	return gameTime;
}

String GameClock::ToString(Time time)
{
	s32 milliseconds = time.AsMilliseconds();
	s32 secs = time.AsSeconds();
	s32 mins = (secs < 60) ? 0 : secs / 60;
	secs %= 60;
	String m = Strings::ToString(mins);
	if (mins < 10)
	{
		m = "0" + m;
	}
	String s = Strings::ToString(secs);
	if (secs < 10)
	{
		s = "0" + s;
	}
	String _m = Strings::ToString(milliseconds);
	if (milliseconds < 10)
	{
		_m = "00" + _m;
	}
	else if (milliseconds < 100)
	{
		_m = "0" + _m;
	}
	if (mins == 0)
	{
		return s + ":" + _m;
	}
	return m + ":" + s + ":" + _m;
}

void GameClock::Tick(Time dt)
{
	gameTime += dt;
}

void GameClock::Reset()
{
	gameTime = Time::Zero;
}
} // namespace LittleEngine
