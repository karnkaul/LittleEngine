#pragma once
#include "StdTypes.h"

namespace Core
{
// \brief Fixed point POD using constant scale factor
struct Fixed final
{
public:
	static const u32 SCALE_FACTOR = 100000;
	static const Fixed Zero;
	static const Fixed One;
	static const Fixed Two;
	static const Fixed Three;
	static const Fixed Ten;
	static const Fixed OneHalf;
	static const Fixed OneThird;
	static const Fixed OneTenth;

private:
	s32 value = 0;

public:
	Fixed() = default;
	explicit Fixed(f32);
	explicit Fixed(f64);
	Fixed(const Fixed&);
	Fixed(Fixed&&) = default;
	Fixed& operator=(const Fixed&);
	Fixed& operator=(Fixed&&) = default;

	Fixed(s32 numerator, s32 denominator = 1);
	u32 ToU32() const;
	s32 ToS32() const;
	f32 ToF32() const;
	f64 ToF64() const;

	Fixed& operator+=(const Fixed& rhs);
	Fixed& operator-=(const Fixed& rhs);
	Fixed& operator*=(const Fixed& rhs);
	Fixed& operator/=(const Fixed& rhs);
	Fixed& operator%=(const Fixed& rhs);
	Fixed& operator++();
	Fixed operator++(int);
	Fixed& operator--();
	Fixed operator--(int);

	Fixed Abs() const;
	Fixed Power(f32 exponent) const;
	Fixed Sqrt() const;
	Fixed Inverse() const;
	// TODO: Need lookup tables for determinism
	Fixed Sin() const;
	Fixed Cos() const;
	Fixed Tan() const;

	bool operator==(const Fixed& rhs) const;
	bool operator>(const Fixed& rhs) const;
	bool operator<(const Fixed& rhs) const;
	bool operator<=(const Fixed& rhs) const;
	bool operator>=(const Fixed& rhs) const;

	String ToString() const;

private:
	struct RawInit
	{
	};

	friend std::ostream& operator<<(std::ostream&, Fixed);
	friend Fixed operator-(const Fixed& rhs);

	Fixed(RawInit, s32 rawFixedValue) : value(rawFixedValue)
	{
	}
};

Fixed operator+(const Fixed& lhs, const Fixed& rhs);
Fixed operator-(const Fixed& lhs, const Fixed& rhs);
Fixed operator*(const Fixed& lhs, const Fixed& rhs);
Fixed operator/(const Fixed& lhs, const Fixed& rhs);
Fixed operator%(const Fixed& lhs, const Fixed& rhs);
bool operator!=(const Fixed& lhs, const Fixed& rhs);
} // namespace Core
