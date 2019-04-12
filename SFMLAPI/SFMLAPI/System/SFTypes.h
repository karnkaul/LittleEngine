#pragma once
#include "CoreTypes.h"
#include "SFML/Graphics.hpp"
#include "SFMLAPI/Rendering/Colour.h"
#include "SFMLAPI/Windowing/SFWindowData.h"

namespace LittleEngine
{
f32 Cast(Fixed fixed);
Fixed Cast(f32 value);

sf::Vector2f Cast(Vector2 vec2);
Vector2 Cast(const sf::Vector2f& vec2);
Vector2 Cast(const sf::Vector2i& vec2);

sf::Color Cast(Colour colour);
Colour Cast(const sf::Color& sfColor);

sf::Uint8 Cast(SFWindowStyle style);
} // namespace LittleEngine
