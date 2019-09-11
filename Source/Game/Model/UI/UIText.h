#pragma once
#include "Core/CoreTypes.h"
#include "SFMLAPI/Rendering/Colour.h"

namespace LittleEngine
{
struct UIText
{
	const static u16 s_DEFAULT_PIXEL_SIZE = 30;

	u16 pixelSize = s_DEFAULT_PIXEL_SIZE;
	String text;
	Colour colour;

	UIText();
	UIText(const char* text);
	UIText(String text);
	UIText(String text, u16 pixelSize, Colour colour);
	UIText(UIText&&) = default;
	UIText& operator=(UIText&&) = default;
};
} // namespace LittleEngine
