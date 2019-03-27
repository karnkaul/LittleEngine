#include "stdafx.h"
#include "SFInputDataFrame.h"

namespace LittleEngine
{
KeyState::KeyState(KeyCode keyCode, const String& name)
	: name(name), keyCode(keyCode), bPressed(false)
{
}

KeyCode KeyState::GetKeyCode() const
{
	return keyCode;
}

const String& KeyState::GetName() const
{
	return name;
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