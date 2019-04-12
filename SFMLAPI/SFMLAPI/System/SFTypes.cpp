#include "stdafx.h"
#include "SFTypes.h"

namespace LittleEngine
{
f32 Cast(Fixed fixed)
{
	return fixed.ToF32();
}

Fixed Cast(f32 value)
{
	return Fixed(value);
}

sf::Vector2f Cast(Vector2 vec2)
{
	return sf::Vector2f(vec2.x.ToF32(), vec2.y.ToF32());
}

Vector2 Cast(const sf::Vector2f& vec2)
{
	return Vector2(Fixed(vec2.x), Fixed(vec2.y));
}

Vector2 Cast(const sf::Vector2i& vec2)
{
	return Vector2(vec2.x, vec2.y);
}

sf::Color Cast(Colour colour)
{
	return sf::Color(colour.r.ToUInt(), colour.g.ToUInt(), colour.b.ToUInt(), colour.a.ToUInt());
}

Colour Cast(const sf::Color& sfColor)
{
	return Colour(sfColor.r, sfColor.g, sfColor.b, sfColor.a);
}
} // namespace LittleEngine
