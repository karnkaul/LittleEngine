#include "SFML/Window/Clipboard.hpp"
#include "InputDataFrame.h"

namespace LittleEngine
{
KeyState::KeyState(s32 keyType, std::string_view name) : keyType(keyType), name(name), bPressed(false) {}

KeyType KeyState::GetKeyType() const
{
	return static_cast<KeyType>(keyType);
}

std::string_view KeyState::Name() const
{
	return name;
}

bool TextInput::ContainsChar(char c) const
{
	size_t idx = text.find(c);
	return idx != std::string::npos;
}

bool TextInput::ContainsKey(s32 keyCode) const
{
	return metaText.find(static_cast<KeyType>(keyCode)) != metaText.end();
}

void TextInput::Reset()
{
	text.clear();
	metaText.clear();
}

std::string InputDataFrame::Clipboard()
{
	return sf::Clipboard::getString();
}
} // namespace LittleEngine
