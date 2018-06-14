#include "stdafx.h"
#include <iostream>
#include <string>

#include "Utils/Fixed.h"

Fixed::Fixed(double value) {
	m_value = static_cast<int>(value * SCALE_FACTOR);
	if ((value * static_cast<double>(SCALE_FACTOR)) - static_cast<double>(m_value) >= 0.5f) {
		m_value += 1;
	}
}

Fixed::Fixed(float value) {
	m_value = static_cast<int>(value * SCALE_FACTOR);
	if ((value * static_cast<double>(SCALE_FACTOR)) - static_cast<double>(m_value) >= 0.5f) {
		m_value += 1;
	}
}

Fixed::Fixed(const Fixed& other) {
	m_value = other.m_value;
}

Fixed& Fixed::operator=(const Fixed& other) {
	m_value = other.m_value;
	return *this;
}

Fixed::Fixed(int numerator, int denominator) {
	m_value = (static_cast<int64_t>(numerator) * static_cast<int64_t>(SCALE_FACTOR) / static_cast<int64_t>(denominator));
}

int Fixed::GetInt() const {
	int floor = static_cast<int>(GetDouble());
	if ((floor * static_cast<double>(SCALE_FACTOR)) - static_cast<double>(m_value) >= 0.5f) {
		return floor + 1;
	}
	return floor;
}

float Fixed::GetFloat() const {
	return static_cast<float>(m_value) / static_cast<float>(SCALE_FACTOR);
}

double Fixed::GetDouble() const {
	return static_cast<double>(m_value) / static_cast<double>(SCALE_FACTOR);
}

Fixed & Fixed::operator+=(const Fixed & rhs) {
	m_value += rhs.m_value;
	return *this;
}

Fixed & Fixed::operator-=(const Fixed & rhs) {
	m_value -= rhs.m_value;
	return *this;
}

Fixed & Fixed::operator*=(const Fixed & rhs) {
	int64_t largeVal = static_cast<int64_t>(m_value) * static_cast<int64_t>(rhs.m_value);
	m_value = static_cast<int>(largeVal / SCALE_FACTOR);
	return *this;
}

Fixed& Fixed::operator/=(const Fixed & rhs) {
	if (rhs.m_value == 0) {
		m_value = 0;
	}
	else {
		int64_t largeVal = (int64_t)m_value * SCALE_FACTOR;
		m_value = static_cast<int>(largeVal / rhs.m_value);
	}	
	return *this;
}

Fixed & Fixed::operator%=(const Fixed & rhs) {
	m_value %= rhs.m_value;
	return *this;
}

Fixed& Fixed::operator++() {
	++m_value;
	return *this;
}

Fixed Fixed::operator++(int) {
	Fixed temporary(*this);
	++*this;
	return temporary;
}


Fixed& Fixed::operator--() {
	--m_value;
	return *this;
}

Fixed Fixed::operator--(int) {
	Fixed temporary(*this);
	--*this;
	return temporary;
}

Fixed Fixed::Abs() const {
	return Fixed(RawInit(), (m_value < 0) ? -m_value : m_value);
}

Fixed Fixed::Power(int exponent) const {
	Fixed ret = *this;
	if (exponent < 0) {
		ret = Inverse();
		exponent = -exponent;
	}
	return Fixed(std::pow(ret.GetDouble(), exponent));
}

Fixed Fixed::Sqrt() const {
	return Fixed(std::sqrt(GetDouble()));
}

Fixed Fixed::Inverse() const {
	return Fixed(SCALE_FACTOR, m_value);
}

Fixed Fixed::Sin() const {
	return Fixed(std::sin(GetDouble()));
}

Fixed Fixed::Cos() const {
	return Fixed(std::cos(GetDouble()));
}

Fixed Fixed::Tan() const {
	return Fixed(std::tan(GetDouble()));
}

Fixed Fixed::operator+(const Fixed& rhs) const {
	return Fixed(*this) += rhs;
}

Fixed Fixed::operator-(const Fixed& rhs) const {
	return Fixed(*this) -= rhs;
}

Fixed Fixed::operator*(const Fixed& rhs) const {
	return Fixed(*this) *= rhs;
}

Fixed Fixed::operator/(const Fixed& rhs) const {
	return Fixed(*this) /= rhs;
}

Fixed Fixed::operator%(const Fixed & rhs) const {
	return Fixed(*this) %= rhs;
}

bool Fixed::operator==(const Fixed & rhs) const {
	return m_value == rhs.m_value;
}

bool Fixed::operator!=(const Fixed & rhs) const {
	return !(*this == rhs);
}

bool Fixed::operator>(const Fixed & rhs) const {
	return m_value > rhs.m_value;
}

bool Fixed::operator<(const Fixed & rhs) const {
	return m_value < rhs.m_value;
}

bool Fixed::operator<=(const Fixed & rhs) const {
	return m_value <= rhs.m_value;
}

bool Fixed::operator>=(const Fixed & rhs) const {
	return m_value >= rhs.m_value;
}

std::string Fixed::ToString() const {
	return std::to_string(GetDouble());
}

Fixed operator-(const Fixed& rhs) {
	return Fixed(Fixed::RawInit(), -rhs.m_value);
}

std::ostream& operator<<(std::ostream& lhs, Fixed rhs) {
	return lhs << rhs.GetDouble();
}
