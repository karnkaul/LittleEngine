#include "Colour.h"

namespace LittleEngine
{
const Colour Colour::Black(0, 0, 0);
const Colour Colour::White;
const Colour Colour::Red(255, 0, 0);
const Colour Colour::Green(0, 255, 0);
const Colour Colour::Blue(0, 0, 255);
const Colour Colour::Yellow(255, 255, 0);
const Colour Colour::Magenta(255, 0, 255);
const Colour Colour::Cyan(0, 255, 255);
const Colour Colour::Transparent(0, 0, 0, 0);

Colour Colour::Lerp(Colour min, Colour max, Fixed alpha)
{
	if (min == max)
	{
		return min;
	}
	return ((Fixed::One - alpha) * min) + (alpha * max);
}

Colour Colour::Lerp(const Core::TRange<Colour>& range, Fixed alpha)
{
	return Lerp(range.min, range.max, alpha);
}

Colour::Colour(UByte r, UByte g, UByte b, UByte a) : r(r), g(g), b(b), a(a) {}

Colour& Colour::operator+=(Colour rhs)
{
	r += rhs.r;
	g += rhs.g;
	b += rhs.b;
	a += rhs.a;
	return *this;
}

Colour& Colour::operator-=(Colour rhs)
{
	r -= rhs.r;
	g -= rhs.g;
	b -= rhs.b;
	a -= rhs.a;
	return *this;
}

std::string Colour::ToString() const
{
	static constexpr size_t MAX = 1 + 3 + 1 + 3 + 1 + 3 + 1 + 3 + 1;
	std::string ret;
	ret.reserve(MAX);
	ret += "[";
	ret += r.ToString();
	ret += ", ";
	ret += g.ToString();
	ret += ", ";
	ret += b.ToString();
	ret += ", ";
	ret += a.ToString();
	ret += "]";
	return ret;
}

Colour operator+(Colour lhs, Colour rhs)
{
	return Colour(lhs) += rhs;
}

Colour operator-(Colour lhs, Colour rhs)
{
	return Colour(lhs) -= rhs;
}

Colour& operator*=(Fixed nCoefficient, Colour& c)
{
	f32 n = Maths::Clamp01(nCoefficient).ToF32();
	c.r = n * c.r;
	c.g = n * c.g;
	c.b = n * c.b;
	c.a = n * c.a;
	return c;
}

Colour operator*(Fixed nCoefficient, Colour colour)
{
	Colour c(colour);
	return nCoefficient *= c;
}

bool operator==(Colour lhs, Colour rhs)
{
	return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

bool operator!=(Colour lhs, Colour rhs)
{
	return !(lhs == rhs);
}
} // namespace LittleEngine
