#pragma once
#include "Utils/Fixed.h"

struct Vector2 {
public:
	Fixed x;
	Fixed y;
	Vector2() : x(0), y(0) {}
	Vector2(Fixed x, Fixed y) : x(x), y(y) {}

	static const Vector2 Zero;
	static const Vector2 One;

	Vector2& operator+=(const Vector2& rhs);
	Vector2& operator-=(const Vector2& rhs);
	Vector2& operator*=(const Fixed& fixed);
	Vector2& operator/=(const Fixed& fixed);
	
	Vector2 operator+(const Vector2& rhs) const;
	Vector2 operator-(const Vector2& rhs) const;
	Vector2 operator*(const Fixed& rhs) const;
	Vector2 operator/(const Fixed& rhs) const;
	Fixed Dot(const Vector2& rhs) const;

	bool operator==(const Vector2& rhs) const;
	bool operator!=(const Vector2& rhs) const;

	Vector2 Normalised() const;
	void Normalise();
	Fixed Magnitude() const;
	Fixed SqrMagnitude() const;

	std::string ToString() const;
	friend std::ostream& operator<<(std::ostream& out, Vector2& vector2) {
		return out << "[" << vector2.x << ", " << vector2.y << "]";
	}
private:
};
