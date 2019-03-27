#include "stdafx.h"
#include "LittleEngine/Engine/EngineService.h"
#include "LittleEngine/Services/Services.h"
#include "KeyboardInput.h"

namespace LittleEngine
{
void LiveLine::Append(const String& newInput)
{
	if (!newInput.empty())
		liveString += newInput;
}

void LiveLine::Backspace()
{
	liveString = liveString.substr(0, liveString.size() - 1);
}

void LiveLine::Clear()
{
	liveString.clear();
}

void KeyboardInput::Update(const EngineInput::Frame& frame)
{
	bool bAppendText = false;

	if (frame.IsHeld(GameInputType::RB) && frame.textInput.Contains(SpecialInputType::Insert))
	{
		m_liveLine.Append(frame.GetClipboard());
	}

	else if (frame.textInput.Contains(SpecialInputType::Backspace))
	{
		m_liveLine.Backspace();
	}

	else if (m_bClearOnEscape && frame.textInput.Contains(SpecialInputType::Escape))
	{
		m_liveLine.Clear();
	}

	else
	{
		bAppendText = true;
	}

	if (bAppendText && !frame.textInput.text.empty())
	{
		m_liveLine.Append(frame.textInput.text);
	}
}

void KeyboardInput::Clear()
{
	m_liveLine.Clear();
}

String KeyboardInput::GetLiveString() const
{
	return m_liveLine.liveString;
}
} // namespace LittleEngine