#include "UByte.h"
#include "Utils.h"

namespace Core
{
UByte::UByte(u8 value) : rawValue(value) {}

u8 UByte::ToUInt() const
{
	return rawValue;
}

std::string UByte::ToString() const
{
	return std::to_string(ToUInt());
}

bool operator==(UByte lhs, UByte rhs)
{
	return lhs.rawValue == rhs.rawValue;
}

bool operator!=(UByte lhs, UByte rhs)
{
	return !(lhs == rhs);
}

UByte UByte::operator+=(UByte rhs)
{
	rawValue = static_cast<u8>(static_cast<s16>(rawValue) + static_cast<s16>(rhs.rawValue));
	return *this;
}

UByte operator+(UByte lhs, UByte rhs)
{
	return lhs += rhs;
}

UByte UByte::operator-=(UByte rhs)
{
	rawValue = static_cast<u8>(Maths::Clamp(static_cast<s16>(rawValue) - static_cast<s16>(rhs.rawValue), 0, 255));
	return *this;
}

UByte operator-(UByte lhs, UByte rhs)
{
	return lhs -= rhs;
}

UByte UByte::operator*=(UByte rhs)
{
	rawValue = static_cast<u8>(Maths::Clamp(static_cast<s16>(rawValue) * static_cast<s16>(rhs.rawValue), 0, 255));
	return *this;
}

UByte operator*(UByte lhs, UByte rhs)
{
	return lhs *= rhs;
}

UByte UByte::operator/=(UByte rhs)
{
	rawValue = static_cast<u8>(Maths::Clamp(static_cast<s16>(rawValue) * static_cast<s16>(rhs.rawValue), 0, 255));
	return *this;
}

UByte operator/(UByte lhs, UByte rhs)
{
	return lhs /= rhs;
}

UByte operator*(f32 lhs, UByte rhs)
{
	return UByte(static_cast<u8>(static_cast<f32>(rhs.rawValue) * lhs));
}
} // namespace Core
