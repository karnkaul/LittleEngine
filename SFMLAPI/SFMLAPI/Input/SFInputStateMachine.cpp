#include "stdafx.h"
#include "SFInputStateMachine.h"
#include "SFMLAPI/System/SFTypes.h"

namespace LittleEngine
{
namespace
{
UMap<u32, KeyCode> asciiIgnoreMap = {
	{8, KeyCode::Backspace}, {9, KeyCode::Tab},		{13, KeyCode::Enter},
	{27, KeyCode::Escape},   {15, KeyCode::LShift}, {16, KeyCode::RShift},
};
}

KeyState& SFInputStateMachine::GetOrCreateKeyState(KeyType key)
{
	for (auto& keyState : m_keyStates)
	{
		if (keyState.GetKeyType() == key)
		{
			return keyState;
		}
	}
	m_keyStates.emplace_back(key);
	return m_keyStates.back();
}

const Fixed SFInputStateMachine::JOY_DEADZONE = Fixed(25);

SFInputStateMachine::SFInputStateMachine()
{
	// Pre-defined keys
	m_keyStates = {
		{KeyCode::Left, "Left"},
		{KeyCode::Right, "Right"},
		{KeyCode::Up, "Up"},
		{KeyCode::Down, "Down"},
		{KeyCode::W, "W"},
		{KeyCode::A, "A"},
		{KeyCode::S, "S"},
		{KeyCode::D, "D"},
		{KeyCode::Space, "Space"},
		{KeyCode::Enter, "Enter"},
		{KeyCode::Escape, "Escape"},
		{KeyCode::Tab, "Tab"},
		{KeyCode::Tilde, "Backtick"},
		{KeyCode::Backspace, "Backspace"},
		{KeyCode::Delete, "Delete"},

		{KeyType::MOUSE_BTN_0, "Mouse0"},
		{KeyType::MOUSE_BTN_1, "Mouse1"},
		{KeyType::MOUSE_BTN_2, "Mouse2"},
		{KeyType::MOUSE_BTN_3, "Mouse3"},
		{KeyType::MOUSE_BTN_4, "Mouse4"},

		{KeyType::JOY_BTN_0, "Joy0"},
		{KeyType::JOY_BTN_1, "Joy1"},
		{KeyType::JOY_BTN_2, "Joy2"},
		{KeyType::JOY_BTN_3, "Joy3"},
		{KeyType::JOY_BTN_4, "Joy4"},
		{KeyType::JOY_BTN_5, "Joy5"},
		{KeyType::JOY_BTN_6, "Joy6"},
		{KeyType::JOY_BTN_7, "Joy7"},
	};
}

SFInputStateMachine::~SFInputStateMachine() = default;

bool SFInputStateMachine::IsKeyPressed(KeyType key) const
{
	for (const auto& iter : m_keyStates)
	{
		if (iter.GetKeyType() == key)
		{
			return iter.bPressed;
		}
	}
	return false;
}

const KeyState* SFInputStateMachine::GetKeyState(KeyType key) const
{
	for (const auto& iter : m_keyStates)
	{
		if (iter.GetKeyType() == key)
		{
			return &iter;
		}
	}
	return nullptr;
}

void SFInputStateMachine::SetInputMapping(InputMap inputMap)
{
	m_inputMap = std::move(inputMap);
}

SFInputDataFrame SFInputStateMachine::GetFrameInputData() const
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
	frame.joyInput = m_joyInput;
	return frame;
}

void SFInputStateMachine::OnKeyDown(const sf::Event::KeyEvent& key)
{
	KeyState& toModify = GetOrCreateKeyState(static_cast<KeyType>(key.code));
	toModify.bPressed = true;
}

void SFInputStateMachine::OnKeyUp(const sf::Event::KeyEvent& key)
{
	KeyState& toModify = GetOrCreateKeyState(static_cast<KeyType>(key.code));
	toModify.bPressed = false;
}

void SFInputStateMachine::OnMouseDown(const sf::Event::MouseButtonEvent& button)
{
	KeyState& toModify = GetOrCreateKeyState(Cast(button.button));
	toModify.bPressed = true;
}

void SFInputStateMachine::OnMouseUp(const sf::Event::MouseButtonEvent& button)
{
	KeyState& toModify = GetOrCreateKeyState(Cast(button.button));
	toModify.bPressed = false;
}

void SFInputStateMachine::OnMouseMove(Vector2 worldPosition)
{
	m_pointerInput.worldPosition = worldPosition;
}

void SFInputStateMachine::OnMouseChange(bool bInViewport)
{
	m_pointerInput.bInViewport = bInViewport;
}

void SFInputStateMachine::SetMouseWheelScroll(Fixed delta)
{
	m_pointerInput.scrollDelta = delta;
}

void SFInputStateMachine::ResetKeyStates()
{
	for (auto& keyState : m_keyStates)
	{
		keyState.bPressed = false;
	}
	m_joyInput.m_states.clear();
	m_pointerInput.scrollDelta = Fixed::Zero;
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
	else if (m_textInput.ignoredChars.find(iter->second) == m_textInput.ignoredChars.end())
	{
		m_textInput.ignoredChars.emplace(iter->second);
	}
}

void SFInputStateMachine::UpdateJoyInput()
{
	m_joyInput.m_states.clear();
	for (u8 sfJoystickID = 0; sfJoystickID < g_MAX_JOYSTICKS; ++sfJoystickID)
	{
		if (sf::Joystick::isConnected(sfJoystickID))
		{
			JoyState state;
			state.id = sfJoystickID;
			u32 buttonCount = sf::Joystick::getButtonCount(sfJoystickID);
			for (u32 sfButtonID = 0; sfButtonID < buttonCount; ++sfButtonID)
			{
				KeyType key = m_inputMap.GetKeyType(sfButtonID);
				KeyState& toModify = GetOrCreateKeyState(key);
				toModify.bPressed = sf::Joystick::isButtonPressed(sfJoystickID, sfButtonID);
				state.pressed.push_back(static_cast<KeyType>(key));
			}
			state.xy.x = Fixed(sf::Joystick::getAxisPosition(sfJoystickID, sf::Joystick::X));
			state.xy.y = -Fixed(sf::Joystick::getAxisPosition(sfJoystickID, sf::Joystick::Y));
			state.zr.x = Fixed(sf::Joystick::getAxisPosition(sfJoystickID, sf::Joystick::Z));
			state.zr.y = Fixed(sf::Joystick::getAxisPosition(sfJoystickID, sf::Joystick::R));
			state.uv.x = Fixed(sf::Joystick::getAxisPosition(sfJoystickID, sf::Joystick::U));
			state.uv.y = -Fixed(sf::Joystick::getAxisPosition(sfJoystickID, sf::Joystick::V));
			state.pov.x = Fixed(sf::Joystick::getAxisPosition(sfJoystickID, sf::Joystick::PovX));
			state.pov.y = Fixed(sf::Joystick::getAxisPosition(sfJoystickID, sf::Joystick::PovY));
			m_joyInput.m_states.emplace_back(std::move(state));
		}
	}
}
} // namespace LittleEngine
