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

sf::Color Cast(Colour colour)
{
	return sf::Color(colour.r.ToUInt(), colour.g.ToUInt(), colour.b.ToUInt(), colour.a.ToUInt());
}

Colour Cast(const sf::Color& sfColor)
{
	return Colour(sfColor.r, sfColor.g, sfColor.b, sfColor.a);
}

sf::Uint8 Cast(SFWindowStyle style)
{
	switch (style)
	{
	default:
		return sf::Style::Close;

	case SFWindowStyle::Bordlerless:
		return sf::Style::None;
	}
}

Vector2 WorldToScreen(Vector2 worldPoint)
{
	return Vector2(worldPoint.x, -worldPoint.y);
}

Fixed WorldToScreen(Fixed worldOrientation)
{
	return -worldOrientation;
}

Vector2 ScreenToWorld(Vector2 screenPoint)
{
	return Vector2(screenPoint.x, -screenPoint.y);
}

Fixed ScreenToWorld(Fixed screenOrientation)
{
	return -screenOrientation;
}
} // namespace LittleEngine
