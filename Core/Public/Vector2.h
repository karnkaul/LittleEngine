#pragma once
#include "Fixed.h"

namespace Core
{
// \brief 2D geometric vector using Fixed
struct Vector2
{
public:
	static const Vector2 Zero;
	static const Vector2 One;

	static Vector2 ToOrientation(Fixed degrees);

public:
	Fixed x;
	Fixed y;

public:
	Vector2() : x(0), y(0)
	{
	}
	Vector2(Fixed x, Fixed y) : x(x), y(y)
	{
	}
	Vector2(const Vector2& other) = default;
	Vector2& operator=(const Vector2&) = default;
	Vector2(Vector2&&) = default;
	Vector2& operator=(Vector2&&) = default;

	Vector2& operator+=(const Vector2& rhs);
	Vector2& operator-=(const Vector2& rhs);
	Vector2& operator*=(const Fixed& fixed);
	Vector2& operator/=(const Fixed& fixed);

	Fixed Dot(const Vector2& rhs) const;

	Vector2 Normalised() const;
	void Normalise();
	Fixed Magnitude() const;
	f64 SqrMagnitude() const;

	String ToString() const;
};

std::ostream& operator<<(std::ostream& out, Vector2& vector2);
Vector2 operator-(const Vector2& lhs);
Vector2 operator+(const Vector2& lhs, const Vector2& rhs);
Vector2 operator-(const Vector2& lhs, const Vector2& rhs);
Vector2 operator*(const Fixed& lhs, const Vector2& rhs);
Vector2 operator*(const Vector2& lhs, const Fixed& rhs);
Vector2 operator/(const Vector2& lhs, const Fixed& rhs);
bool operator==(const Vector2& lhs, const Vector2& rhs);
bool operator!=(const Vector2& lhs, const Vector2& rhs);
} // namespace Core
