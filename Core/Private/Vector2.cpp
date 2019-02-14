#include "stdafx.h"
#include "Vector2.h"
#include "Utils.h"
#include <math.h>
#include <string>

namespace Core
{
const Vector2 Vector2::Zero = Vector2(0, 0);
const Vector2 Vector2::One = Vector2(1, 1);

Vector2 Vector2::ToOrientation(Fixed degrees)
{
	return Vector2((Maths::DEG_TO_RAD * degrees).Sin(), (Maths::DEG_TO_RAD * degrees).Cos());
}

Vector2& Vector2::operator+=(const Vector2& rhs)
{
	x += rhs.x;
	y += rhs.y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}

Vector2& Vector2::operator*=(const Fixed& fixed)
{
	x *= fixed;
	y *= fixed;
	return *this;
}

Vector2& Vector2::operator/=(const Fixed& fixed)
{
	x /= fixed;
	y /= fixed;
	return *this;
}

Fixed Vector2::Dot(const Vector2& rhs) const
{
	return Fixed((x * rhs.x) + (y * rhs.y));
}

Vector2 Vector2::Normalised() const
{
	f64 sq = SqrMagnitude();
	if (sq == 0)
		return Vector2::Zero;
	Fixed coeff = Fixed(std::sqrt(sq));
	coeff = coeff.Inverse();
	return Vector2(x * coeff, y * coeff);
}

void Vector2::Normalise()
{
	*this = Normalised();
}

Fixed Vector2::Magnitude() const
{
	return Fixed(std::sqrt(SqrMagnitude()));
}

f64 Vector2::SqrMagnitude() const
{
	return (x.ToF64() * x.ToF64()) + (y.ToF64() * y.ToF64());
}

String Vector2::ToString() const
{
	return "[" + x.ToString() + ", " + y.ToString() + "]";
}

Vector2 operator-(const Vector2& lhs)
{
	return Vector2(-lhs.x, -lhs.y);
}

Vector2 operator+(const Vector2& lhs, const Vector2& rhs)
{
	return Vector2(lhs) += rhs;
}

Vector2 operator-(const Vector2& lhs, const Vector2& rhs)
{
	return Vector2(lhs) -= rhs;
}

Vector2 operator*(const Fixed& lhs, const Vector2& rhs)
{
	return Vector2(rhs) *= lhs;
}

Vector2 operator*(const Vector2& lhs, const Fixed& rhs)
{
	return Vector2(lhs) *= rhs;
}

Vector2 operator/(const Vector2& lhs, const Fixed& rhs)
{
	return Vector2(lhs) /= rhs;
}

std::ostream& operator<<(std::ostream& out, Vector2& vector2)
{
	return out << "[" << vector2.x << ", " << vector2.y << "]";
}

bool operator==(const Vector2& lhs, const Vector2& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator!=(const Vector2& lhs, const Vector2& rhs)
{
	return !(lhs == rhs);
}
} // namespace Core
