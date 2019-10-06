#pragma once
#include "StdTypes.h"
#include "Fixed.h"

namespace Core
{
struct Time
{
private:
	s64 microSeconds;

public:
	static const Time Zero;
	static std::string ToString(Time time);
	static Time Microseconds(s64 microSeconds);
	static Time Milliseconds(s32 milliSeconds);
	static Time Seconds(f32 seconds);
	static Time Now();
	static Time Clamp(Time val, Time min, Time max);
	static void Reset();

	constexpr Time() : microSeconds(0) {}
	explicit constexpr Time(s64 microSeconds) : microSeconds(microSeconds) {}

	Time& Scale(Fixed magnitude);
	Time Scaled(Fixed magnitude) const;

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
};

Time operator+(const Time& lhs, const Time& rhs);
Time operator-(const Time& lhs, const Time& rhs);
Time operator*(const Time& lhs, const Time& rhs);
Time operator/(const Time& lhs, const Time& rhs);
} // namespace Core
