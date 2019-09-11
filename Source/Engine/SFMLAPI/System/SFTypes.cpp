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

sf::Uint8 Cast(ViewportStyle style)
{
	switch (style)
	{
	default:
		return sf::Style::Close;

	case ViewportStyle::Bordlerless:
		return sf::Style::None;
	}
}

KeyType Cast(sf::Mouse::Button button)
{
	switch (button)
	{
	default:
		return static_cast<KeyType>(-1);
	case sf::Mouse::Button::Left:
		return KeyType::MOUSE_BTN_0;

	case sf::Mouse::Button::Right:
		return KeyType::MOUSE_BTN_1;

	case sf::Mouse::Button::Middle:
		return KeyType::MOUSE_BTN_2;

	case sf::Mouse::Button::XButton1:
		return KeyType::MOUSE_BTN_3;

	case sf::Mouse::Button::XButton2:
		return KeyType::MOUSE_BTN_4;
	}
}

Vector2 WorldToViewport(Vector2 worldPoint)
{
	return Vector2(worldPoint.x, -worldPoint.y);
}

Fixed WorldToViewport(Fixed worldOrientation)
{
	return -worldOrientation;
}

Vector2 ViewportToWorld(Vector2 vpPoint)
{
	return Vector2(vpPoint.x, -vpPoint.y);
}

Fixed ViewportToWorld(Fixed vpOrientation)
{
	return -vpOrientation;
}
} // namespace LittleEngine
