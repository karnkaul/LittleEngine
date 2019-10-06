#include "Matrix3.h"

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
	if (magnitude != Fixed::Zero)
	{
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;
	}
}

Vector3 Vector3::Normalised() const
{
	Vector3 ret = *this;
	ret.Normalise();
	return ret;
}

const Matrix3 Matrix3::ZERO =
	Matrix3({Fixed::Zero, Fixed::Zero, Fixed::Zero}, {Fixed::Zero, Fixed::Zero, Fixed::Zero}, {Fixed::Zero, Fixed::Zero, Fixed::Zero});

void Matrix3::Normalise()
{
	m_x.Normalise();
	m_y.Normalise();
	m_w.z = Fixed::One;
}
} // namespace Core
