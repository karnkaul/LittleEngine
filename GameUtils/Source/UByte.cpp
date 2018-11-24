#include "UByte.h"
namespace GameUtils {
	UByte::UByte(unsigned int value) : rawValue(static_cast<char>(value)) {
	}

	unsigned int UByte::ToUInt() const {
		return static_cast<unsigned int>(rawValue);
	}

	std::string UByte::ToString() const {
		return std::to_string(ToUInt());
	}

	bool operator==(UByte lhs, UByte rhs) {
		return lhs.rawValue == rhs.rawValue;
	}

	bool operator!=(UByte lhs, UByte rhs) {
		return !(lhs == rhs);
	}

	UByte operator+=(UByte lhs, UByte rhs) {
		lhs.rawValue += rhs.rawValue;
		return lhs;
	}

	UByte operator+(UByte lhs, UByte rhs) {
		return lhs += rhs;
	}

	UByte operator-=(UByte lhs, UByte rhs) {
		lhs.rawValue -= rhs.rawValue;
		return lhs;
	}

	UByte operator-(UByte lhs, UByte rhs) {
		return lhs -= rhs;
	}

	UByte operator*=(UByte lhs, UByte rhs) {
		lhs.rawValue *= rhs.rawValue;
		return lhs;
	}

	UByte operator*(UByte lhs, UByte rhs) {
		return lhs *= rhs;
	}

	UByte operator/=(UByte lhs, UByte rhs) {
		lhs.rawValue /= rhs.rawValue;
		return lhs;
	}

	UByte operator/(UByte lhs, UByte rhs) {
		return lhs /= rhs;
	}
}
