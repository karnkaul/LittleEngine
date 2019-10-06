#include <array>
#include <iostream>
#include <string>
#include "../LECoreUtils/Asserts.h"
#include "../LECoreUtils/Maths.h"
#include "Fixed.h"

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

u32 Fixed::ToU32() const
{
	f64 val = ToF64();
	if (val < 0)
	{
		val = -val;
	}
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

Fixed Fixed::ArcSin() const
{
	f64 val = ToF64();
	Assert(val >= -1.0 && val <= 1.0, "Out of range");
	return Fixed(std::asin(val));
}

Fixed Fixed::Cos() const
{
	return Fixed(std::cos(ToF64()));
}

Fixed Fixed::ArcCos() const
{
	f64 val = ToF64();
	Assert(val >= -1.0 && val <= 1.0, "Out of range");
	return Fixed(std::acos(val));
}

Fixed Fixed::Tan() const
{
	return Fixed(std::tan(ToF64()));
}

Fixed Fixed::ArcTan() const
{
	f64 val = ToF64();
	if (val == 1.0)
	{
		return Maths::DEG_TO_RAD * Fixed(90);
	}
	return Fixed(std::atan(val));
}

std::string Fixed::ToString() const
{
	std::array<char, 16> buf;
	SPRINTF(buf.data(), buf.size(), "%.2f", ToF32());
	return std::string(buf.data());
}
} // namespace Core
