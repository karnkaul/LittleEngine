#include "stdafx.h"
#include "SFInputStateMachine.h"

namespace LittleEngine
{
namespace
{
UMap<u32, SpecialInputType> asciiIgnoreMap = {
	{8, SpecialInputType::Backspace}, {9, SpecialInputType::Tab},	{13, SpecialInputType::Enter},
	{27, SpecialInputType::Escape},   {15, SpecialInputType::Shift}, {16, SpecialInputType::Shift},
};
}

const KeyMod KeyMod::Default = KeyMod();

KeyMod::KeyMod() = default;
KeyMod::KeyMod(bool bControl, bool bAlt, bool bShift)
	: bControl(bControl), bAlt(bAlt), bShift(bShift)
{
}
KeyMod::KeyMod(const sf::Event::KeyEvent& event)
	: bControl(event.control), bAlt(event.alt), bShift(event.shift)
{
}

KeyState& SFInputStateMachine::GetOrCreateKeyState(KeyCode code)
{
	for (auto& keyState : m_keyStates)
	{
		if (keyState.GetKeyCode() == code)
		{
			return keyState;
		}
	}
	m_keyStates.emplace_back(code);
	return *m_keyStates.rbegin();
}

SFInputStateMachine::SFInputStateMachine()
{
	// Pre-defined keys
	m_keyStates.emplace_back(KeyCode::Left, "Left");
	m_keyStates.emplace_back(KeyCode::Right, "Right");
	m_keyStates.emplace_back(KeyCode::Up, "Up");
	m_keyStates.emplace_back(KeyCode::Down, "Down");
	m_keyStates.emplace_back(KeyCode::W, "W");
	m_keyStates.emplace_back(KeyCode::A, "A");
	m_keyStates.emplace_back(KeyCode::S, "S");
	m_keyStates.emplace_back(KeyCode::D, "D");
	m_keyStates.emplace_back(KeyCode::Space, "Space");
	m_keyStates.emplace_back(KeyCode::Enter, "Enter");
	m_keyStates.emplace_back(KeyCode::Escape, "Escape");
	m_keyStates.emplace_back(KeyCode::Tab, "Tab");
	m_keyStates.emplace_back(KeyCode::Tilde, "Backtick");
	m_keyStates.emplace_back(KeyCode::Backspace, "Backspace");
}

SFInputStateMachine::~SFInputStateMachine() = default;

bool SFInputStateMachine::IsKeyPressed(KeyCode code) const
{
	for (const auto& iter : m_keyStates)
	{
		if (iter.GetKeyCode() == code)
		{
			return iter.bPressed;
		}
	}
	return false;
}

const KeyState SFInputStateMachine::GetKeyState(KeyCode code) const
{
	for (const auto& iter : m_keyStates)
	{
		if (iter.GetKeyCode() == code)
		{
			return iter;
		}
	}
	return KeyState(code);
}

const SFInputDataFrame SFInputStateMachine::GetFrameInputData() const
{
	SFInputDataFrame frame;
	for (const auto& iter : m_keyStates)
	{
		if (iter.bPressed)
		{
			frame.pressed.emplace_back(iter);
		}
	}
	frame.textInput = m_textInput;
	frame.mouseInput = m_pointerInput;
	return frame;
}

void SFInputStateMachine::OnKeyDown(const sf::Event::KeyEvent& key)
{
	KeyState& toModify = GetOrCreateKeyState(key.code);
	StoreNonASCIISpecialInput(key.code);
	toModify.bPressed = true;
}

void SFInputStateMachine::OnKeyUp(const sf::Event::KeyEvent& key)
{
	KeyState& toModify = GetOrCreateKeyState(key.code);
	toModify.bPressed = false;
}

void SFInputStateMachine::SetPointerState(MouseInput pointerInput)
{
	this->m_pointerInput = std::move(pointerInput);
}

void SFInputStateMachine::ResetKeyStates()
{
	for (auto& keyState : m_keyStates)
	{
		keyState.bPressed = false;
	}
}

void SFInputStateMachine::ClearTextInput()
{
	m_textInput.Reset();
}

void SFInputStateMachine::OnTextInput(u32 unicode)
{
	if (unicode < 5 || unicode >= 128)
	{
		return;
	}

	auto iter = asciiIgnoreMap.find(unicode);
	if (iter == asciiIgnoreMap.end())
	{
		m_textInput.text += static_cast<char>(unicode);
	}
}

void SFInputStateMachine::StoreNonASCIISpecialInput(KeyCode key)
{
	switch (key)
	{
	case KeyCode::Up:
		m_textInput.specials.push_back(SpecialInputType::Up);
		break;

	case KeyCode::Down:
		m_textInput.specials.push_back(SpecialInputType::Down);
		break;

	case KeyCode::Left:
		m_textInput.specials.push_back(SpecialInputType::Left);
		break;

	case KeyCode::Right:
		m_textInput.specials.push_back(SpecialInputType::Right);
		break;

	case KeyCode::Tab:
		m_textInput.specials.push_back(SpecialInputType::Tab);
		break;

	case KeyCode::Enter:
		m_textInput.specials.push_back(SpecialInputType::Enter);
		break;

	case KeyCode::BackSpace:
		m_textInput.specials.push_back(SpecialInputType::Backspace);
		break;

	case KeyCode::Escape:
		m_textInput.specials.push_back(SpecialInputType::Escape);
		break;

	case KeyCode::LShift:
	case KeyCode::RShift:
		m_textInput.specials.push_back(SpecialInputType::Shift);
		break;

	case KeyCode::LControl:
	case KeyCode::RControl:
		m_textInput.specials.push_back(SpecialInputType::Control);
		break;

	case KeyCode::LAlt:
	case KeyCode::RAlt:
		m_textInput.specials.push_back(SpecialInputType::Alt);
		break;

	case KeyCode::Insert:
		m_textInput.specials.push_back(SpecialInputType::Insert);
		break;

	case KeyCode::Delete:
		m_textInput.specials.push_back(SpecialInputType::Delete);
		break;

	case KeyCode::PageUp:
		m_textInput.specials.push_back(SpecialInputType::PageUp);
		break;

	case KeyCode::PageDown:
		m_textInput.specials.push_back(SpecialInputType::PageDown);
		break;

	case KeyCode::Home:
		m_textInput.specials.push_back(SpecialInputType::Home);
		break;

	case KeyCode::End:
		m_textInput.specials.push_back(SpecialInputType::End);
		break;

	default:
		break;
	}
}
} // namespace LittleEngine
