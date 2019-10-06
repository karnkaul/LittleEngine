#pragma once
#include "Vector2.h"

namespace Core
{
struct Vector3
{
	Fixed x;
	Fixed y;
	Fixed z;

	Fixed SqrMagnitude() const;
	Fixed Magnitude() const;

	void Normalise();
	Vector3 Normalised() const;

	inline void Set(Vector2 xy)
	{
		x = xy.x;
		y = xy.y;
	}

	inline void Set(Vector2 xy, Fixed z)
	{
		Set(xy);
		this->z = z;
	}
};

// Row-major
class Matrix3
{
public:
	Vector3 m_x; // [1, 0, 0]
	Vector3 m_y; // [0, 1, 0]
	Vector3 m_w; // [0, 0, 1]

public:
	static const Matrix3 ZERO;

public:
	Matrix3();
	Matrix3(Vector3 x, Vector3 y, Vector3 w);
	Matrix3(Vector2 position, Vector2 orientation, Vector2 scale);
	Matrix3(Vector2 position, Fixed rotation, Vector2 scale);

	Matrix3& operator*=(const Matrix3& rhs);
	void Normalise();

	Vector2 Position() const;
	Vector2 Orientation() const;
};

Matrix3 operator*(const Matrix3& lhs, const Matrix3& rhs);
Vector2 operator*(Vector2 lhs, const Matrix3& rhs);

inline Matrix3::Matrix3()
	: m_x({Fixed::One, Fixed::Zero, Fixed::Zero}), m_y({Fixed::Zero, Fixed::One, Fixed::Zero}), m_w({Fixed::Zero, Fixed::Zero, Fixed::One})
{
}

inline Matrix3::Matrix3(Vector3 x, Vector3 y, Vector3 w) : m_x(std::move(x)), m_y(std::move(y)), m_w(std::move(w)) {}

inline Matrix3::Matrix3(Vector2 position, Vector2 orientation, Vector2 scale)
{
	if (scale.x == scale.y)
	{
		Fixed s = scale.x;
		m_x.Set({orientation.x * s, orientation.y * s});
		m_y.Set({-orientation.y * s, orientation.x * s});
	}
	else
	{
		Matrix3 rot;
		rot.m_x.Set({orientation.x, orientation.y});
		rot.m_y.Set({-orientation.y, orientation.x});
		m_x.Set({scale.x, 0});
		m_y.Set({0, scale.y});
		*this *= rot;
	}
	m_w.Set(position, Fixed::One);
}

inline Matrix3::Matrix3(Vector2 position, Fixed rotation, Vector2 scale)
{
	Vector2 orientation = Vector2::ToOrientation(rotation);
	*this = Matrix3(position, orientation, scale);
}

inline Matrix3& Matrix3::operator*=(const Matrix3& rhs)
{
	Matrix3 c = *this;

	m_x.x = c.m_x.x * rhs.m_x.x + c.m_x.y * rhs.m_y.x + c.m_x.z * rhs.m_w.x;
	m_x.y = c.m_x.x * rhs.m_x.y + c.m_x.y * rhs.m_y.y + c.m_x.z * rhs.m_w.y;
	m_x.z = c.m_x.x * rhs.m_x.z + c.m_x.y * rhs.m_y.z + c.m_x.z * rhs.m_w.z;

	m_y.x = c.m_y.x * rhs.m_x.x + c.m_y.y * rhs.m_y.x + c.m_y.z * rhs.m_w.x;
	m_y.y = c.m_y.x * rhs.m_x.y + c.m_y.y * rhs.m_y.y + c.m_y.z * rhs.m_w.y;
	m_y.z = c.m_y.x * rhs.m_x.z + c.m_y.y * rhs.m_y.z + c.m_y.z * rhs.m_w.z;

	m_w.x = c.m_w.x * rhs.m_x.x + c.m_w.y * rhs.m_y.x + c.m_w.z * rhs.m_w.x;
	m_w.y = c.m_w.x * rhs.m_x.y + c.m_w.y * rhs.m_y.y + c.m_w.z * rhs.m_w.y;
	m_w.z = c.m_w.x * rhs.m_x.z + c.m_w.z * rhs.m_y.z + c.m_w.z * rhs.m_w.z;

	return *this;
}

inline Vector2 Matrix3::Position() const
{
	return {m_w.x, m_w.y};
}

inline Vector2 Matrix3::Orientation() const
{
	return {m_x.x, m_x.y};
}

inline Matrix3 operator*(const Matrix3& lhs, const Matrix3& rhs)
{
	Matrix3 ret = lhs;
	return ret *= rhs;
}

inline Vector2 operator*(Vector2 lhs, const Matrix3& rhs)
{
	return {lhs.x * rhs.m_x.x + lhs.y * rhs.m_y.x + rhs.m_w.x, lhs.x * rhs.m_x.y + lhs.y * rhs.m_y.y + rhs.m_w.y};
}
} // namespace Core
