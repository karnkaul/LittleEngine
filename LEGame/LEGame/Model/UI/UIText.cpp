#include "stdafx.h"
#include "UIText.h"

namespace LittleEngine
{
UIText::UIText() : colour(Colour::Black)
{
}
UIText::UIText(const char* text) : text(String(text)), colour(Colour::Black)
{
}
UIText::UIText(String text) : text(std::move(text)), colour(Colour::Black)
{
}
UIText::UIText(String text, u32 pixelSize, Colour colour)
	: pixelSize(pixelSize), text(std::move(text)), colour(colour)
{
}
} // namespace LittleEngine
