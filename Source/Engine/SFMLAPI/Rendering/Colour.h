#pragma once
#include "Core/CoreTypes.h"
#include "Core/TRange.h"

namespace LittleEngine
{
// \brief Compressed wrapper struct for SFML Color
struct Colour
{
public:
	static const Colour Black;
	static const Colour White;
	static const Colour Red;
	static const Colour Green;
	static const Colour Blue;
	static const Colour Yellow;
	static const Colour Magenta;
	static const Colour Cyan;
	static const Colour Transparent;

	static Colour Lerp(Colour min, Colour max, Fixed alpha);
	static Colour Lerp(const Core::TRange<Colour>& range, Fixed alpha);

	UByte r;
	UByte g;
	UByte b;
	UByte a;

	// Warning: All inputs must be unsigned ints!
	Colour(UByte r = 255, UByte g = 255, UByte b = 255, UByte a = 255);

	Colour& operator+=(Colour rhs);
	Colour& operator-=(Colour rhs);

	String ToString() const;
};

Colour operator+(Colour lhs, Colour rhs);
Colour operator-(Colour lhs, Colour rhs);
Colour& operator*=(Fixed nCoefficient, Colour& colour);
Colour operator*(Fixed nCoefficient, Colour colour);

bool operator==(Colour lhs, Colour rhs);
bool operator!=(Colour lhs, Colour rhs);
} // namespace LittleEngine
