#include <array>
#include <cmath>
#include <string>
#include "Vector2.h"
#include "Utils.h"

namespace Core
{
const Vector2 Vector2::Zero = Vector2(Fixed::Zero, Fixed::Zero);
const Vector2 Vector2::One = Vector2(Fixed::One, Fixed::One);
const Vector2 Vector2::Up = Vector2(Fixed::Zero, Fixed::One);
const Vector2 Vector2::Right = Vector2(Fixed::One, Fixed::Zero);

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

Vector2 Vector2::Rotate(Vector2 orientation, Fixed degrees) 
{
	Fixed c = (degrees * Maths::DEG_TO_RAD).Cos();
	Fixed s = (degrees * Maths::DEG_TO_RAD).Sin();
	Fixed x = orientation.x * c - orientation.y * s;
	Fixed y = orientation.x * s + orientation.y * c;
	return {x, y};
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

std::string Vector2::ToString() const
{
	std::array<char, 40> buf;
	SPRINTF(buf.data(), buf.size(), "(%.2f, %.2f)", x.ToF32(), y.ToF32());
	return std::string(buf.data());
}
} // namespace Core
