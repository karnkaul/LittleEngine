#pragma once
#include "CoreTypes.h"

namespace sf
{
class Time;
}

namespace LittleEngine
{
struct Time
{
private:
	s64 microSeconds;

public:
	static const Time Zero;
	static String ToString(Time time);
	static Time Microseconds(s64 microSeconds);
	static Time Milliseconds(s32 milliSeconds);
	static Time Seconds(f32 seconds);
	static Time Now();
	static Time Clamp(Time val, Time min, Time max);
	static void RestartRealtimeClock();

	Time();
	explicit Time(s64 microSeconds);
	explicit Time(sf::Time& sfTime);

	Time& Scale(const Fixed& magnitude);

	Time& operator-();
	Time& operator+=(const Time& rhs);
	Time& operator-=(const Time& rhs);
	Time& operator*=(const Time& rhs);
	Time& operator/=(const Time& rhs);

	bool operator==(const Time& rhs);
	bool operator!=(const Time& rhs);
	bool operator<(const Time& rhs);
	bool operator<=(const Time& rhs);
	bool operator>(const Time& rhs);
	bool operator>=(const Time& rhs);

	f32 AsSeconds() const;
	s32 AsMilliseconds() const;
	s64 AsMicroseconds() const;

	sf::Time _ToSFTime() const;
};

Time operator+(const Time& lhs, const Time& rhs);
Time operator-(const Time& lhs, const Time& rhs);
Time operator*(const Time& lhs, const Time& rhs);
Time operator/(const Time& lhs, const Time& rhs);
} // namespace LittleEngine
