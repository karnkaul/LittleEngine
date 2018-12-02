#pragma once
#include <string>

namespace GameUtils {
	// \brief 1-byte unsigned integer structure, useful to store values between 0-255
	struct UByte {
		unsigned char rawValue;

		// Any literals passed in must be positive!
		UByte(unsigned int value = 0);

		// Use on the stack when a full int is required
		unsigned int ToUInt() const;
		std::string ToString() const;
	};

	bool operator==(UByte lhs, UByte rhs);
	bool operator!=(UByte lhs, UByte rhs);
	UByte operator+=(UByte lhs, UByte rhs);
	UByte operator+(UByte lhs, UByte rhs);
	UByte operator-=(UByte lhs, UByte rhs);
	UByte operator-(UByte lhs, UByte rhs);
	UByte operator*=(UByte lhs, UByte rhs);
	UByte operator*(UByte lhs, UByte rhs);
	UByte operator/=(UByte lhs, UByte rhs);
	UByte operator/(UByte lhs, UByte rhs);
}
