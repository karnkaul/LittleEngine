#include "stdafx.h"
#include <string>
#include <math.h>
#include "Vector2.h"

Vector2 Vector2::operator+(const Vector2& rhs) const {
	return Vector2(*this) += rhs;
}

Vector2 Vector2::operator-(const Vector2& rhs) const {
	return Vector2(*this) -= rhs;
}

Vector2& Vector2::operator+=(const Vector2& rhs) {
	x += rhs.x;
	y += rhs.y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& rhs) {
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}

Vector2& Vector2::operator*=(const Fixed& fixed) {
	x *= fixed;
	y *= fixed;
	return *this;
}

Vector2& Vector2::operator/=(const Fixed& fixed) {
	x /= fixed;
	y /= fixed;
	return *this;
}

Vector2 Vector2::operator*(const Fixed& rhs) const {
	return Vector2(*this) *= rhs;
}

Vector2 Vector2::operator/(const Fixed & rhs) const {
	return Vector2(*this) /= rhs;
}

Fixed Vector2::Dot(const Vector2 & rhs) const {
	return Fixed((x * rhs.x) + (y * rhs.y));
}

bool Vector2::operator==(const Vector2& rhs) const {
	return x == rhs.x && y == rhs.y;
}

bool Vector2::operator!=(const Vector2& rhs) const {
	return !(*this == rhs);
}

#include <iostream>

Vector2 Vector2::Normalised() const {
	std::cout << " sqrt(x + y): " << std::sqrt(x.GetInt() + y.GetInt());
	Fixed sq = (x * x) + (y * y);
	Fixed coeff = Fixed(std::sqrt(sq.GetDouble()));
	std::cout << " sqrt(x + y): " << coeff;
	coeff = coeff.Inverse();
	std::cout << " inv: " << coeff << "\n";
	return Vector2(x * coeff, y * coeff);
}

void Vector2::Normalise() {
	*this = Normalised();
}

Fixed Vector2::Magnitude() const {
	return SqrMagnitude().Sqrt();
}

Fixed Vector2::SqrMagnitude() const {
	return Fixed((x * x) + (y * y));
}

std::string Vector2::ToString() const {
	return "[" + x.ToString() + ", " + y.ToString() + "]";
}
