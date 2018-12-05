#include <iostream>
#include <string>
#include "Fixed.h"

namespace GameUtils {
	const Fixed Fixed::Zero = Fixed(0);
	const Fixed Fixed::One = Fixed(1);
	const Fixed Fixed::Two = Fixed(2);
	const Fixed Fixed::Three = Fixed(3);
	const Fixed Fixed::Ten = Fixed(10);
	const Fixed Fixed::OneHalf = Fixed(1, 2);
	const Fixed Fixed::OneThird = Fixed(1, 3);
	const Fixed Fixed::OneTenth = Fixed(1, 10);

	Fixed::Fixed(double value) : value(static_cast<int>(value * SCALE_FACTOR)) {
		if ((value * static_cast<double>(SCALE_FACTOR)) - static_cast<double>(value) >= 0.5f) {
			value += 1;
		}
	}

	Fixed::Fixed(float value) : value(static_cast<int>(value * SCALE_FACTOR)) {
		if ((value * static_cast<double>(SCALE_FACTOR)) - static_cast<double>(value) >= 0.5f) {
			value += 1;
		}
	}

	Fixed::Fixed(const Fixed& other) : value(other.value) {}

	Fixed& Fixed::operator=(const Fixed& other) {
		value = other.value;
		return *this;
	}

	Fixed::Fixed(int numerator, int denominator) :
		value(static_cast<int64_t>(numerator) * static_cast<int64_t>(SCALE_FACTOR) / static_cast<int64_t>(denominator))
	{}

	int Fixed::ToInt() const {
		int floor = static_cast<int>(ToDouble());
		if ((floor * static_cast<double>(SCALE_FACTOR)) - static_cast<double>(value) >= 0.5f) {
			return floor + 1;
		}
		return floor;
	}

	float Fixed::ToFloat() const {
		return static_cast<float>(value) / static_cast<float>(SCALE_FACTOR);
	}

	double Fixed::ToDouble() const {
		return static_cast<double>(value) / static_cast<double>(SCALE_FACTOR);
	}

	Fixed & Fixed::operator+=(const Fixed & rhs) {
		value += rhs.value;
		return *this;
	}

	Fixed & Fixed::operator-=(const Fixed & rhs) {
		value -= rhs.value;
		return *this;
	}

	Fixed & Fixed::operator*=(const Fixed & rhs) {
		int64_t largeVal = static_cast<int64_t>(value) * static_cast<int64_t>(rhs.value);
		value = static_cast<int>(largeVal / SCALE_FACTOR);
		return *this;
	}

	Fixed& Fixed::operator/=(const Fixed & rhs) {
		if (rhs.value == 0) {
			value = 0;
		}
		else {
			int64_t largeVal = (int64_t)value * SCALE_FACTOR;
			value = static_cast<int>(largeVal / rhs.value);
		}
		return *this;
	}

	Fixed & Fixed::operator%=(const Fixed & rhs) {
		value %= rhs.value;
		return *this;
	}

	Fixed& Fixed::operator++() {
		++value;
		return *this;
	}

	Fixed Fixed::operator++(int) {
		Fixed temporary(*this);
		++*this;
		return temporary;
	}


	Fixed& Fixed::operator--() {
		--value;
		return *this;
	}

	Fixed Fixed::operator--(int) {
		Fixed temporary(*this);
		--*this;
		return temporary;
	}

	Fixed Fixed::Abs() const {
		return Fixed(RawInit(), (value < 0) ? -value : value);
	}

	Fixed Fixed::Power(int exponent) const {
		Fixed ret = *this;
		if (exponent < 0) {
			ret = Inverse();
			exponent = -exponent;
		}
		return Fixed(std::pow(ret.ToDouble(), exponent));
	}

	Fixed Fixed::Sqrt() const {
		return Fixed(std::sqrt(ToDouble()));
	}

	Fixed Fixed::Inverse() const {
		return Fixed(SCALE_FACTOR, value);
	}

	Fixed Fixed::Sin() const {
		return Fixed(std::sin(ToDouble()));
	}

	Fixed Fixed::Cos() const {
		return Fixed(std::cos(ToDouble()));
	}

	Fixed Fixed::Tan() const {
		return Fixed(std::tan(ToDouble()));
	}

	bool Fixed::operator==(const Fixed & rhs) const {
		return value == rhs.value;
	}

	bool Fixed::operator>(const Fixed & rhs) const {
		return value > rhs.value;
	}

	bool Fixed::operator<(const Fixed & rhs) const {
		return value < rhs.value;
	}

	bool Fixed::operator<=(const Fixed & rhs) const {
		return value <= rhs.value;
	}

	bool Fixed::operator>=(const Fixed & rhs) const {
		return value >= rhs.value;
	}

	std::string Fixed::ToString() const {
		return std::to_string(ToDouble());
	}

	Fixed operator-(const Fixed& rhs) {
		return Fixed(Fixed::RawInit(), -rhs.value);
	}

	std::ostream& operator<<(std::ostream& lhs, Fixed rhs) {
		return lhs << rhs.ToDouble();
	}

	Fixed operator+(const Fixed& lhs, const Fixed& rhs) {
		return Fixed(lhs) += rhs;
	}

	Fixed operator-(const Fixed& lhs, const Fixed& rhs) {
		return Fixed(lhs) -= rhs;
	}

	Fixed operator*(const Fixed& lhs, const Fixed& rhs) {
		return Fixed(lhs) *= rhs;
	}

	Fixed operator/(const Fixed& lhs, const Fixed& rhs) {
		return Fixed(lhs) /= rhs;
	}

	Fixed operator%(const Fixed& lhs, const Fixed & rhs) {
		return Fixed(lhs) %= rhs;
	}

	bool operator!=(const Fixed& lhs, const Fixed & rhs) {
		return !(lhs == rhs);
	}
}
