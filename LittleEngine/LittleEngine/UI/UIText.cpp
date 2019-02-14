#include "stdafx.h"
#include "UIText.h"

namespace LittleEngine
{
UIText::UIText() : pixelSize(30), colour(Colour::Black)
{
}
UIText::UIText(const char* text) : pixelSize(30), text(String(text)), colour(Colour::Black)
{
}
UIText::UIText(const String& text) : pixelSize(30), text(text), colour(Colour::Black)
{
}
UIText::UIText(String&& text) : pixelSize(30), text(std::move(text)), colour(Colour::Black)
{
}
UIText::UIText(const String& text, u32 pixelSize, Colour colour)
	: pixelSize(pixelSize), text(text), colour(colour)
{
}
} // namespace LittleEngine
