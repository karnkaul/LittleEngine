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
	explicit Fixed(f32 value);
	explicit Fixed(f64 value);
	Fixed(const Fixed& rhs);
	Fixed(Fixed&&) = default;
	Fixed& operator=(const Fixed& rhs);
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
	Fixed Sin() const;
	Fixed ArcSin() const;
	Fixed Cos() const;
	Fixed ArcCos() const;
	Fixed Tan() const;
	Fixed ArcTan() const;

	bool operator==(const Fixed& rhs) const;
	bool operator>(const Fixed& rhs) const;
	bool operator<(const Fixed& rhs) const;
	bool operator<=(const Fixed& rhs) const;
	bool operator>=(const Fixed& rhs) const;

	String ToString() const;

private:
	friend std::ostream& operator<<(std::ostream& os, Fixed rhs);
	friend Fixed operator-(const Fixed& rhs);
};

Fixed operator+(const Fixed& lhs, const Fixed& rhs);
Fixed operator-(const Fixed& lhs, const Fixed& rhs);
Fixed operator*(const Fixed& lhs, const Fixed& rhs);
Fixed operator/(const Fixed& lhs, const Fixed& rhs);
Fixed operator%(const Fixed& lhs, const Fixed& rhs);
bool operator!=(const Fixed& lhs, const Fixed& rhs);
} // namespace Core
