#pragma once
#include "SFMLAPI/Rendering/Colour.h"
#include "SFMLAPI/Rendering/SFLayerID.h"

namespace LittleEngine
{
struct UIStyle
{
	Vector2 size = Vector2::Zero;
	Fixed border = Fixed::Zero;
	Colour fill = Colour::Transparent;
	Colour outline = Colour::Transparent;
	LayerID layer = LAYER_UI;
};

struct UIWidgetStyle
{
	UIStyle uninteractable;
	UIStyle notSelected;
	UIStyle selected;
	UIStyle interacting;
	Vector2 widgetSize;
	LayerID baseLayer = LAYER_UI;
	Colour background;
};
} // namespace LittleEngine
