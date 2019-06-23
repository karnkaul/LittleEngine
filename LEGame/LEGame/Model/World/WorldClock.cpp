#include "stdafx.h"
#include "WorldClock.h"
#include "Core/Utils.h"

namespace LittleEngine
{
Time WorldClock::gameTime = Time::Zero;

WorldClock::WorldClock()
{
	m_startTime = gameTime;
}

WorldClock::~WorldClock() = default;

void WorldClock::Restart()
{
	m_startTime = gameTime;
}

Time WorldClock::GetElapsed() const
{
	return gameTime - m_startTime;
}

Time WorldClock::GetGameTime()
{
	return gameTime;
}

String WorldClock::ToString(Time time)
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

void WorldClock::Tick(Time dt)
{
	gameTime += dt;
}

void WorldClock::Reset()
{
	gameTime = Time::Zero;
}
} // namespace LittleEngine
