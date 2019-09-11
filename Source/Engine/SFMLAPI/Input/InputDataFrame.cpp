#include "SFML/Window/Clipboard.hpp"
#include "InputDataFrame.h"

namespace LittleEngine
{
KeyState::KeyState(s32 keyType, const char* szName) : keyType(keyType), szName(szName), bPressed(false) {}

KeyType KeyState::GetKeyType() const
{
	return static_cast<KeyType>(keyType);
}

const char* KeyState::NameStr() const
{
	return szName;
}

bool TextInput::ContainsChar(char c) const
{
	size_t idx = text.find(c);
	return idx != String::npos;
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

String InputDataFrame::Clipboard()
{
	return sf::Clipboard::getString();
}
} // namespace LittleEngine
