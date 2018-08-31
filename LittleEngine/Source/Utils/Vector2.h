#pragma once
#include "Utils/Fixed.h"

struct Vector2 {
public:
	Fixed x;
	Fixed y;

	static const Vector2 Zero;
	static const Vector2 One;

	Vector2() : x(0), y(0) {}
	Vector2(Fixed x, Fixed y) : x(x), y(y) {}
	Vector2(const Vector2& other) = default;
	Vector2& operator=(const Vector2&) = default;
	Vector2(Vector2&&) = default;
	Vector2& operator=(Vector2&&) = default;

	Vector2& operator+=(const Vector2& rhs);
	Vector2& operator-=(const Vector2& rhs);
	Vector2& operator*=(const Fixed& fixed);
	Vector2& operator/=(const Fixed& fixed);

	Vector2 operator-() const;
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
	double SqrMagnitude() const;

	std::string ToString() const;
	
private:
};

std::ostream& operator<<(std::ostream& out, Vector2& vector2);
