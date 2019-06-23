#include "stdafx.h"
#include "Matrix3.h"
#include "Utils.h"

namespace Core
{
Fixed Vector3::Magnitude() const
{
	return SqrMagnitude().Sqrt();
}

Fixed Vector3::SqrMagnitude() const
{
	return x * x + y * y + z * z;
}

void Vector3::Normalise()
{
	Fixed magnitude = Magnitude();
	Assert(magnitude.ToF32() != 0.0f, "Vector3 magnitude is zero!");
	x /= magnitude;
	y /= magnitude;
	z /= magnitude;
}

Vector3 Vector3::Normalised() const
{
	Vector3 ret = *this;
	ret.Normalise();
	return ret;
}

const Matrix3 Matrix3::ZERO = Matrix3({Fixed::Zero, Fixed::Zero, Fixed::Zero},
									  {Fixed::Zero, Fixed::Zero, Fixed::Zero},
									  {Fixed::Zero, Fixed::Zero, Fixed::Zero});

Matrix3::Matrix3()
	: m_x({Fixed::One, Fixed::Zero, Fixed::Zero}),
	  m_y({Fixed::Zero, Fixed::One, Fixed::Zero}),
	  m_w({Fixed::Zero, Fixed::Zero, Fixed::One})
{
}

Matrix3::Matrix3(Vector3 x, Vector3 y, Vector3 w) : m_x(std::move(x)), m_y(std::move(y)), m_w(std::move(w))
{
}

Matrix3::Matrix3(Vector2 position, Vector2 orientation, Vector2 scale)
{
	orientation.Normalise();
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

Matrix3::Matrix3(Vector2 position, Fixed rotation, Vector2 scale)
{
	Vector2 orientation = Vector2::ToOrientation(rotation);
	*this = Matrix3(position, orientation, scale);
}

Matrix3& Matrix3::operator*=(const Matrix3& rhs)
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

void Matrix3::Normalise()
{
	m_x.Normalise();
	m_y.Normalise();
	m_w.z = Fixed::One;
}

Matrix3 operator*(const Matrix3& lhs, const Matrix3& rhs)
{
	Matrix3 ret = lhs;
	return ret *= rhs;
}

Vector2 operator*(Vector2 lhs, const Matrix3& rhs)
{
	return {lhs.x * rhs.m_x.x + lhs.y * rhs.m_y.x + rhs.m_w.x,
			lhs.x * rhs.m_x.y + lhs.y * rhs.m_y.y + rhs.m_w.y};
}
} // namespace Core
