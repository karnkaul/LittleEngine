#pragma once
#include "CoreTypes.h"
#include "SFMLAPI/Rendering/Colour.h"

namespace LittleEngine
{
struct UIText
{
	u32 pixelSize;
	String text;
	Colour colour;

	UIText();
	UIText(const char* text);
	UIText(const String& text);
	UIText(String&& text);
	UIText(const String& text, u32 pixelSize, Colour colour);
};
} // namespace LittleEngine
