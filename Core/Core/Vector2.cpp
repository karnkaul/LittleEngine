#include "stdafx.h"
#include "Vector2.h"
#include "Utils.h"
#include <cmath>
#include <string>
#include <iomanip>

namespace Core
{
const Vector2 Vector2::Zero = Vector2(Fixed::Zero, Fixed::Zero);
const Vector2 Vector2::One = Vector2(Fixed::One, Fixed::One);
const Vector2 Vector2::Up = Vector2(Fixed::Zero, Fixed::One);
const Vector2 Vector2::Right = Vector2(Fixed::One, Fixed::Zero);

Vector2::Vector2() : x(Fixed::Zero), y(Fixed::Zero)
{
}

Vector2::Vector2(Fixed x, Fixed y) : x(std::move(x)), y(std::move(y))
{
}

Vector2 Vector2::ToOrientation(Fixed degrees)
{
	Fixed rad = Maths::DEG_TO_RAD * degrees;
	return Vector2(rad.Cos(), rad.Sin());
}

Fixed Vector2::ToOrientation(Vector2 orientation)
{
	if (orientation.SqrMagnitude() < 0.001)
	{
		return Fixed::Zero;
	}
	orientation.Normalise();
	Fixed cos = orientation.Dot(Right);
	Fixed sin = orientation.Dot(Up);
	Fixed degrees;
	// +/- 90
	if (cos == Fixed::Zero)
	{
		if (sin < Fixed::Zero)
		{
			degrees = -90;
		}
		else
		{
			degrees = 90;
		}
	}
	else
	{
		// Will only return [-90, 90]
		degrees = (sin / cos).ArcTan() * Maths::RAD_TO_DEG;
	}
	if (orientation.x < Fixed::Zero)
	{
		// So add 180 if facing left
		degrees += 180;
	}
	return degrees;
}

Vector2& Vector2::operator+=(Vector2 rhs)
{
	x += rhs.x;
	y += rhs.y;
	return *this;
}

Vector2& Vector2::operator-=(Vector2 rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}

Vector2& Vector2::operator*=(Fixed fixed)
{
	x *= fixed;
	y *= fixed;
	return *this;
}

Vector2& Vector2::operator/=(Fixed fixed)
{
	x /= fixed;
	y /= fixed;
	return *this;
}

Fixed Vector2::Dot(Vector2 rhs) const
{
	return (x * rhs.x) + (y * rhs.y);
}

Vector2 Vector2::Normalised() const
{
	f64 sq = SqrMagnitude();
	if (sq == 0)
	{
		return Vector2::Zero;
	}
	auto coeff = Fixed(std::sqrt(sq)).Inverse();
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
	StringStream ret;
	ret << std::fixed << std::setprecision(3);
	ret << "[" << x.ToF64() << ", " << y.ToF64() << "]";
	return ret.str();
}

Vector2 operator-(Vector2 lhs)
{
	return Vector2(-lhs.x, -lhs.y);
}

Vector2 operator+(Vector2 lhs, Vector2 rhs)
{
	return Vector2(lhs) += rhs;
}

Vector2 operator-(Vector2 lhs, Vector2 rhs)
{
	return Vector2(lhs) -= rhs;
}

Vector2 operator*(Fixed lhs, Vector2 rhs)
{
	return Vector2(rhs) *= lhs;
}

Vector2 operator*(Vector2 lhs, Fixed rhs)
{
	return Vector2(lhs) *= rhs;
}

Vector2 operator/(Vector2 lhs, Fixed rhs)
{
	return Vector2(lhs) /= rhs;
}

std::ostream& operator<<(std::ostream& out, Vector2& vector2)
{
	return out << "[" << vector2.x << ", " << vector2.y << "]";
}

bool operator==(Vector2 lhs, Vector2 rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator!=(Vector2 lhs, Vector2 rhs)
{
	return !(lhs == rhs);
}
} // namespace Core
