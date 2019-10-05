#pragma once
#include <string>
#include <ostream>
#include "StdTypes.h"

namespace Core
{
// \brief Fixed point POD using constant scale factor
struct Fixed final
{
public:
	static constexpr u32 SCALE_FACTOR = 10000;
	static const Fixed Zero;
	static const Fixed One;
	static const Fixed Two;
	static const Fixed Three;
	static const Fixed Ten;
	static const Fixed OneHalf;
	static const Fixed OneThird;
	static const Fixed OneTenth;

private:
	s32 value;

public:
	constexpr Fixed() : value(0) {}
	constexpr explicit Fixed(f32 value) : value(static_cast<s32>(value * SCALE_FACTOR)) {}
	constexpr explicit Fixed(f64 value) : value(static_cast<s32>(value * SCALE_FACTOR)) {}
	constexpr Fixed(s32 numerator, s32 denominator = 1)
		: value(static_cast<s32>(static_cast<s64>(numerator) * static_cast<s64>(SCALE_FACTOR) / static_cast<s64>(denominator))){};

	u32 ToU32() const;
	s32 ToS32() const;
	f32 ToF32() const;
	f64 ToF64() const;

	Fixed& operator+=(Fixed rhs);
	Fixed& operator-=(Fixed rhs);
	Fixed& operator*=(Fixed rhs);
	Fixed& operator/=(Fixed rhs);
	Fixed& operator%=(Fixed rhs);
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

	bool operator==(Fixed rhs) const;
	bool operator>(Fixed rhs) const;
	bool operator<(Fixed rhs) const;
	bool operator<=(Fixed rhs) const;
	bool operator>=(Fixed rhs) const;

	std::string ToString() const;

private:
	friend Fixed operator-(Fixed rhs);
};

std::ostream& operator<<(std::ostream& os, Fixed rhs);
Fixed operator+(Fixed lhs, Fixed rhs);
Fixed operator-(Fixed lhs, Fixed rhs);
Fixed operator*(Fixed lhs, Fixed rhs);
Fixed operator/(Fixed lhs, Fixed rhs);
Fixed operator%(Fixed lhs, Fixed rhs);
bool operator!=(Fixed lhs, Fixed rhs);

inline Fixed& Fixed::operator+=(Fixed rhs)
{
	value += rhs.value;
	return *this;
}

inline Fixed& Fixed::operator-=(Fixed rhs)
{
	value -= rhs.value;
	return *this;
}

inline Fixed& Fixed::operator*=(Fixed rhs)
{
	s64 largeVal = static_cast<s64>(value) * static_cast<s64>(rhs.value);
	value = static_cast<s32>(largeVal / SCALE_FACTOR);
	return *this;
}

inline Fixed& Fixed::operator/=(Fixed rhs)
{
	if (rhs.value == 0)
	{
		value = 0;
	}
	else
	{
		s64 largeVal = static_cast<s64>(value) * SCALE_FACTOR;
		value = static_cast<s32>(largeVal / rhs.value);
	}
	return *this;
}

inline Fixed& Fixed::operator%=(Fixed rhs)
{
	value %= rhs.value;
	return *this;
}

inline Fixed& Fixed::operator++()
{
	++value;
	return *this;
}

inline Fixed Fixed::operator++(int)
{
	Fixed temporary(*this);
	++*this;
	return temporary;
}

inline Fixed& Fixed::operator--()
{
	--value;
	return *this;
}

inline Fixed Fixed::operator--(int)
{
	Fixed temporary(*this);
	--*this;
	return temporary;
}

inline Fixed Fixed::Abs() const
{
	return (value < 0) ? -*this : *this;
}

inline bool Fixed::operator==(Fixed rhs) const
{
	return value == rhs.value;
}

inline bool Fixed::operator>(Fixed rhs) const
{
	return value > rhs.value;
}

inline bool Fixed::operator<(Fixed rhs) const
{
	return value < rhs.value;
}

inline bool Fixed::operator<=(Fixed rhs) const
{
	return value <= rhs.value;
}

inline bool Fixed::operator>=(Fixed rhs) const
{
	return value >= rhs.value;
}

inline Fixed operator-(Fixed rhs)
{
	Fixed ret;
	ret.value = -rhs.value;
	return ret;
}

inline std::ostream& operator<<(std::ostream& os, Fixed rhs)
{
	return os << rhs.ToF64();
}

inline Fixed operator+(Fixed lhs, Fixed rhs)
{
	return Fixed(lhs) += rhs;
}

inline Fixed operator-(Fixed lhs, Fixed rhs)
{
	return Fixed(lhs) -= rhs;
}

inline Fixed operator*(Fixed lhs, Fixed rhs)
{
	return Fixed(lhs) *= rhs;
}

inline Fixed operator/(Fixed lhs, Fixed rhs)
{
	return Fixed(lhs) /= rhs;
}

inline Fixed operator%(Fixed lhs, Fixed rhs)
{
	return Fixed(lhs) %= rhs;
}

inline bool operator!=(Fixed lhs, Fixed rhs)
{
	return !(lhs == rhs);
}
} // namespace Core
