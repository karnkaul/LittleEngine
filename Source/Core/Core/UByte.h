#pragma once
#include <string>
#include "StdTypes.h"

namespace Core
{
// \brief 1-byte unsigned integer structure, useful to store values between 0-255
struct UByte
{
	u8 rawValue;

	// Any literals passed in must be positive!
	UByte(u8 value = 0);

	u8 ToUInt() const;
	std::string ToString() const;

	UByte operator+=(UByte rhs);
	UByte operator-=(UByte rhs);
	UByte operator*=(UByte rhs);
	UByte operator/=(UByte rhs);
};

bool operator==(UByte lhs, UByte rhs);
bool operator!=(UByte lhs, UByte rhs);

UByte operator+(UByte lhs, UByte rhs);
UByte operator-(UByte lhs, UByte rhs);
UByte operator*(UByte lhs, UByte rhs);
UByte operator/(UByte lhs, UByte rhs);

UByte operator*(f32 lhs, UByte rhs);
} // namespace Core
