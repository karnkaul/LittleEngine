#pragma once
#include "Core/CoreTypes.h"
#include "SFMLAPI/Rendering/Colour.h"

namespace LittleEngine
{
struct UIText
{
	const static u32 s_DEFAULT_PIXEL_SIZE = 30;

	u32 pixelSize = s_DEFAULT_PIXEL_SIZE;
	String text;
	Colour colour;

	UIText();
	UIText(const char* text);
	UIText(String text);
	UIText(String text, u32 pixelSize, Colour colour);
	UIText(UIText&&) = default;
	UIText& operator=(UIText&&) = default;
};
} // namespace LittleEngine
