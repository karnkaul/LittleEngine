#pragma once
#include "StdTypes.h"
#include "Fixed.h"

namespace Core
{
struct Time
{
public:
	using Fixed = Core::Fixed;

private:
	s64 microSeconds = 0;

public:
	static const Time Zero;
	static String ToString(Time time);
	static Time Microseconds(s64 microSeconds);
	static Time Milliseconds(s32 milliSeconds);
 	static Time Seconds(f32 seconds);
	static Time Now();
	static Time Clamp(Time val, Time min, Time max);
	static void Reset();
	
	Time();
	explicit Time(s64 microSeconds);
	explicit Time(sf::Time& sfTime);

	Time& Scale(Fixed magnitude);

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
} // namespace Core
