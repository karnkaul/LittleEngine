#pragma once
#include "Core/CoreTypes.h"
#include "SFML/Graphics.hpp"
#include "SFMLAPI/Input/InputMappings.h"
#include "SFMLAPI/Rendering/Colour.h"
#include "SFMLAPI/Viewport/ViewportData.h"

namespace LittleEngine
{
f32 Cast(Fixed fixed);
Fixed Cast(f32 value);

Vector2 Cast(const sf::Vector2<u32>& vec2u);
Vector2 Cast(const sf::Vector2i& vec2);
Vector2 Cast(const sf::Vector2f& vec2f);

sf::Vector2f Cast(Vector2 vec2);
sf::Color Cast(Colour colour);
Colour Cast(const sf::Color& sfColor);

sf::Uint8 Cast(ViewportStyle style);

KeyType Cast(sf::Mouse::Button sfButton);

Vector2 WorldToSFML(Vector2 worldPoint);
Fixed WorldToSFML(Fixed worldOrientation);
Vector2 SFMLToWorld(Vector2 vpPoint);
Fixed SFMLToWorld(Fixed vpOrientation);

inline Vector2 Cast(const sf::Vector2<u32>& vec2u)
{
	return Vector2(ToS32(vec2u.x), ToS32(vec2u.y));
}

inline Vector2 Cast(const sf::Vector2i& vec2)
{
	return Vector2(vec2.x, vec2.y);
}

inline Vector2 Cast(const sf::Vector2f& vec2f)
{
	return Vector2(Fixed(vec2f.x), Fixed(vec2f.y));
}

inline f32 Cast(Fixed fixed)
{
	return fixed.ToF32();
}

inline Fixed Cast(f32 value)
{
	return Fixed(value);
}

inline sf::Vector2f Cast(Vector2 vec2)
{
	return sf::Vector2f(vec2.x.ToF32(), vec2.y.ToF32());
}

inline sf::Color Cast(Colour colour)
{
	return sf::Color(colour.r.ToUInt(), colour.g.ToUInt(), colour.b.ToUInt(), colour.a.ToUInt());
}

inline Colour Cast(const sf::Color& sfColor)
{
	return Colour(sfColor.r, sfColor.g, sfColor.b, sfColor.a);
}

inline sf::Uint8 Cast(ViewportStyle style)
{
	switch (style)
	{
	default:
		return sf::Style::Close;

	case ViewportStyle::Bordlerless:
		return sf::Style::None;
	}
}

inline KeyType Cast(sf::Mouse::Button button)
{
	switch (button)
	{
	default:
		return KeyType::KEYTYPE_INVALID;
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

inline Vector2 WorldToSFML(Vector2 worldPoint)
{
	return Vector2(worldPoint.x, -worldPoint.y);
}

inline Fixed WorldToSFML(Fixed worldOrientation)
{
	return -worldOrientation;
}

inline Vector2 SFMLToWorld(Vector2 vpPoint)
{
	return Vector2(vpPoint.x, -vpPoint.y);
}

inline Fixed SFMLToWorld(Fixed vpOrientation)
{
	return -vpOrientation;
}
} // namespace LittleEngine
