#include "stdafx.h"
#include "SFInputDataFrame.h"

namespace LittleEngine
{
KeyState::KeyState(KeyCode keyCode, const char* szName)
	: name(szName), keyCode(keyCode), bPressed(false)
{
}

KeyCode KeyState::GetKeyCode() const
{
	return keyCode;
}

const char* KeyState::GetNameStr() const
{
	return name.c_str();
}

bool TextInput::Contains(char c) const
{
	size_t idx = text.find(c);
	return idx != String::npos;
}

bool TextInput::Contains(SpecialInputType special) const
{
	return Core::VectorSearch(specials, special) != specials.end();
}

void TextInput::Reset()
{
	text.clear();
	specials.clear();
}

String SFInputDataFrame::GetClipboard()
{
	return sf::Clipboard::getString();
}
} // namespace LittleEngine