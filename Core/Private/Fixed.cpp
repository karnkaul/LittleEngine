#include "stdafx.h"
#include "Fixed.h"
#include <iostream>
#include <string>

namespace Core
{
const Fixed Fixed::Zero = Fixed(0, 1);
const Fixed Fixed::One = Fixed(1, 1);
const Fixed Fixed::Two = Fixed(2, 1);
const Fixed Fixed::Three = Fixed(3, 1);
const Fixed Fixed::Ten = Fixed(10, 1);
const Fixed Fixed::OneHalf = Fixed(1, 2);
const Fixed Fixed::OneThird = Fixed(1, 3);
const Fixed Fixed::OneTenth = Fixed(1, 10);

Fixed::Fixed(f32 value) : value(static_cast<s32>(value * SCALE_FACTOR))
{
	if ((value * static_cast<f64>(SCALE_FACTOR)) - static_cast<f64>(value) >= 0.5)
	{
		value += 1;
	}
}

Fixed::Fixed(f64 value) : value(static_cast<s32>(value * SCALE_FACTOR))
{
	if ((value * static_cast<f64>(SCALE_FACTOR)) - static_cast<f64>(value) >= 0.5)
	{
		value += 1;
	}
}

Fixed::Fixed(const Fixed& other) : value(other.value)
{
}

Fixed& Fixed::operator=(const Fixed& other)
{
	value = other.value;
	return *this;
}

Fixed::Fixed(s32 numerator, s32 denominator)
	: value(static_cast<s64>(numerator) * static_cast<s64>(SCALE_FACTOR) / static_cast<s64>(denominator))
{
}

u32 Fixed::ToU32() const
{
	f64 val = ToF64();
	if (val < 0)
		val = -val;
	u32 floor = static_cast<u32>(val);
	if ((floor * static_cast<f64>(SCALE_FACTOR)) - static_cast<f64>(value) >= 0.5)
	{
		return floor + 1;
	}
	return floor;
}

s32 Fixed::ToS32() const
{
	s32 floor = static_cast<s32>(ToF64());
	if ((floor * static_cast<f64>(SCALE_FACTOR)) - static_cast<f64>(value) >= 0.5)
	{
		return floor + 1;
	}
	return floor;
}

f32 Fixed::ToF32() const
{
	return static_cast<f32>(value) / static_cast<f32>(SCALE_FACTOR);
}

f64 Fixed::ToF64() const
{
	return static_cast<f64>(value) / static_cast<f64>(SCALE_FACTOR);
}

Fixed& Fixed::operator+=(const Fixed& rhs)
{
	value += rhs.value;
	return *this;
}

Fixed& Fixed::operator-=(const Fixed& rhs)
{
	value -= rhs.value;
	return *this;
}

Fixed& Fixed::operator*=(const Fixed& rhs)
{
	s64 largeVal = static_cast<s64>(value) * static_cast<s64>(rhs.value);
	value = static_cast<s32>(largeVal / SCALE_FACTOR);
	return *this;
}

Fixed& Fixed::operator/=(const Fixed& rhs)
{
	if (rhs.value == 0)
	{
		value = 0;
	}
	else
	{
		s64 largeVal = (s64)value * SCALE_FACTOR;
		value = static_cast<s32>(largeVal / rhs.value);
	}
	return *this;
}

Fixed& Fixed::operator%=(const Fixed& rhs)
{
	value %= rhs.value;
	return *this;
}

Fixed& Fixed::operator++()
{
	++value;
	return *this;
}

Fixed Fixed::operator++(int)
{
	Fixed temporary(*this);
	++*this;
	return temporary;
}

Fixed& Fixed::operator--()
{
	--value;
	return *this;
}

Fixed Fixed::operator--(int)
{
	Fixed temporary(*this);
	--*this;
	return temporary;
}

Fixed Fixed::Abs() const
{
	return Fixed(RawInit(), (value < 0) ? -value : value);
}

Fixed Fixed::Power(f32 exponent) const
{
	Fixed ret = *this;
	if (exponent < 0)
	{
		ret = Inverse();
		exponent = -exponent;
	}
	return Fixed(std::pow(ret.ToF64(), exponent));
}

Fixed Fixed::Sqrt() const
{
	return Fixed(std::sqrt(ToF64()));
}

Fixed Fixed::Inverse() const
{
	return Fixed(SCALE_FACTOR, value);
}

Fixed Fixed::Sin() const
{
	return Fixed(std::sin(ToF64()));
}

Fixed Fixed::Cos() const
{
	return Fixed(std::cos(ToF64()));
}

Fixed Fixed::Tan() const
{
	return Fixed(std::tan(ToF64()));
}

bool Fixed::operator==(const Fixed& rhs) const
{
	return value == rhs.value;
}

bool Fixed::operator>(const Fixed& rhs) const
{
	return value > rhs.value;
}

bool Fixed::operator<(const Fixed& rhs) const
{
	return value < rhs.value;
}

bool Fixed::operator<=(const Fixed& rhs) const
{
	return value <= rhs.value;
}

bool Fixed::operator>=(const Fixed& rhs) const
{
	return value >= rhs.value;
}

String Fixed::ToString() const
{
	return std::to_string(ToF64());
}

Fixed operator-(const Fixed& rhs)
{
	return Fixed(Fixed::RawInit(), -rhs.value);
}

std::ostream& operator<<(std::ostream& lhs, Fixed rhs)
{
	return lhs << rhs.ToF64();
}

Fixed operator+(const Fixed& lhs, const Fixed& rhs)
{
	return Fixed(lhs) += rhs;
}

Fixed operator-(const Fixed& lhs, const Fixed& rhs)
{
	return Fixed(lhs) -= rhs;
}

Fixed operator*(const Fixed& lhs, const Fixed& rhs)
{
	return Fixed(lhs) *= rhs;
}

Fixed operator/(const Fixed& lhs, const Fixed& rhs)
{
	return Fixed(lhs) /= rhs;
}

Fixed operator%(const Fixed& lhs, const Fixed& rhs)
{
	return Fixed(lhs) %= rhs;
}

bool operator!=(const Fixed& lhs, const Fixed& rhs)
{
	return !(lhs == rhs);
}
} // namespace Core
