#include "UIText.h"

namespace LittleEngine
{
UIText::UIText() : colour(Colour::Black) {}
UIText::UIText(const char* text) : text(std::string(text)), colour(Colour::Black) {}
UIText::UIText(std::string text) : text(std::move(text)), colour(Colour::Black) {}
UIText::UIText(std::string text, u16 pixelSize, Colour colour) : pixelSize(pixelSize), text(std::move(text)), colour(colour) {}
} // namespace LittleEngine
