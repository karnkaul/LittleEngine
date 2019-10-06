#pragma once
#include "Core/CoreTypes.h"
#include "SFMLAPI/Rendering/Colour.h"

namespace LittleEngine
{
struct UIText
{
	const static u16 s_DEFAULT_PIXEL_SIZE = 30;

	u16 pixelSize = s_DEFAULT_PIXEL_SIZE;
	std::string text;
	Colour colour;

	UIText();
	UIText(const char* text);
	UIText(std::string text);
	UIText(std::string text, u16 pixelSize, Colour colour);
	UIText(UIText&&) = default;
	UIText& operator=(UIText&&) = default;
};
} // namespace LittleEngine
