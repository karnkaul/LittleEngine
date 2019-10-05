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
	static const Vector2 Up;
	static const Vector2 Right;

	static Vector2 ToOrientation(Fixed degrees);
	static Fixed ToOrientation(Vector2 orientation);
	static Vector2 Rotate(Vector2 orientation, Fixed degrees);

public:
	Fixed x;
	Fixed y;

public:
	Vector2() = default;
	Vector2(Fixed x, Fixed y);

	Vector2& operator+=(Vector2 rhs);
	Vector2& operator-=(Vector2 rhs);
	Vector2& operator*=(Fixed fixed);
	Vector2& operator/=(Fixed fixed);

	Fixed Dot(Vector2 rhs) const;

	Vector2 Normalised() const;
	void Normalise();
	Fixed Magnitude() const;
	f64 SqrMagnitude() const;

	std::string ToString() const;
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

inline Vector2::Vector2(Fixed x, Fixed y) : x(std::move(x)), y(std::move(y)) {}

inline Vector2& Vector2::operator+=(Vector2 rhs)
{
	x += rhs.x;
	y += rhs.y;
	return *this;
}

inline Vector2& Vector2::operator-=(Vector2 rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}

inline Vector2& Vector2::operator*=(Fixed fixed)
{
	x *= fixed;
	y *= fixed;
	return *this;
}

inline Vector2& Vector2::operator/=(Fixed fixed)
{
	x /= fixed;
	y /= fixed;
	return *this;
}

inline Fixed Vector2::Dot(Vector2 rhs) const
{
	return (x * rhs.x) + (y * rhs.y);
}

inline Vector2 operator-(Vector2 lhs)
{
	return Vector2(-lhs.x, -lhs.y);
}

inline Vector2 operator+(Vector2 lhs, Vector2 rhs)
{
	return lhs += rhs;
}

inline Vector2 operator-(Vector2 lhs, Vector2 rhs)
{
	return lhs -= rhs;
}

inline Vector2 operator*(Fixed lhs, Vector2 rhs)
{
	return rhs *= lhs;
}

inline Vector2 operator*(Vector2 lhs, Fixed rhs)
{
	return lhs *= rhs;
}

inline Vector2 operator/(Vector2 lhs, Fixed rhs)
{
	return lhs /= rhs;
}

inline std::ostream& operator<<(std::ostream& out, Vector2& vector2)
{
	return out << "[" << vector2.x << ", " << vector2.y << "]";
}

inline bool operator==(Vector2 lhs, Vector2 rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(Vector2 lhs, Vector2 rhs)
{
	return !(lhs == rhs);
}
} // namespace Core
