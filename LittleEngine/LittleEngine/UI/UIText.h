#pragma once
#include "CoreTypes.h"
#include "SFMLAPI/Rendering/Colour.h"

namespace LittleEngine
{
struct UIText
{
	u32 pixelSize = 30;
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
