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
	Vector2();
	Vector2(Fixed x, Fixed y);
	Vector2(const Vector2& other) = default;
	Vector2& operator=(const Vector2&) = default;
	Vector2(Vector2&&) = default;
	Vector2& operator=(Vector2&&) = default;

	Vector2& operator+=(Vector2 rhs);
	Vector2& operator-=(Vector2 rhs);
	Vector2& operator*=(Fixed fixed);
	Vector2& operator/=(Fixed fixed);

	Fixed Dot(Vector2 rhs) const;

	Vector2 Normalised() const;
	void Normalise();
	Fixed Magnitude() const;
	f64 SqrMagnitude() const;

	String ToString() const;
};

std::ostream& operator<<(std::ostream& out, Vector2& vector2);
Vector2 operator-(Vector2 lhs);
Vector2 operator+(Vector2 lhs, Vector2 rhs);
Vector2 operator-(Vector2 lhs, Vector2 rhs);
Vector2 operator*(Fixed lhs, Vector2 rhs);
Vector2 operator*(Vector2 lhs, Fixed rhs);
Vector2 operator/(Vector2 lhs, Fixed rhs);
bool operator==(Vector2 lhs, Vector2 rhs);
bool operator!=(Vector2 lhs, Vector2 rhs);
} // namespace Core
