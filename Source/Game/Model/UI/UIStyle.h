#pragma once
#include "SFMLAPI/Rendering/Colour.h"
#include "SFMLAPI/Rendering/LayerID.h"

namespace LittleEngine
{
struct UIStyle
{
	Vector2 size = Vector2::Zero;
	Fixed border = Fixed::Zero;
	Colour fill = Colour::Transparent;
	Colour outline = Colour::Transparent;
	Colour textColour = Colour::Black;
	LayerID layer = LayerID::UI;
};

struct UIWidgetStyle
{
	UIStyle uninteractable;
	UIStyle notSelected;
	UIStyle selected;
	UIStyle interacting;
	Vector2 widgetSize;
	class FontAsset* pFont = nullptr;
	LayerID baseLayer = LayerID::UI;
	Colour background;
};
} // namespace LittleEngine
