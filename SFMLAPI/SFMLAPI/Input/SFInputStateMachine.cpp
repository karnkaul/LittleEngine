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
	for (u8 id = 0; id < g_MAX_JOYSTICKS; ++id)
	{
		if (sf::Joystick::isConnected(id))
		{
			JoyState state;
			state.id = id;
			u32 buttonCount = sf::Joystick::getButtonCount(id);
			s32 key = KeyType::JOY_BTN_0;
			for (u32 btnId = 0; btnId < buttonCount; ++btnId)
			{
				KeyState& toModify = GetOrCreateKeyState(static_cast<KeyType>(key++));
				toModify.bPressed = sf::Joystick::isButtonPressed(id, btnId);
				state.pressed.push_back(static_cast<KeyType>(key));
			}
			state.xy.x = Fixed(sf::Joystick::getAxisPosition(id, sf::Joystick::X));
			state.xy.y = -Fixed(sf::Joystick::getAxisPosition(id, sf::Joystick::Y));
			state.zr.x = Fixed(sf::Joystick::getAxisPosition(id, sf::Joystick::Z));
			state.zr.y = Fixed(sf::Joystick::getAxisPosition(id, sf::Joystick::R));
			state.uv.x = Fixed(sf::Joystick::getAxisPosition(id, sf::Joystick::U));
			state.uv.y = -Fixed(sf::Joystick::getAxisPosition(id, sf::Joystick::V));
			state.pov.x = Fixed(sf::Joystick::getAxisPosition(id, sf::Joystick::PovX));
			state.pov.y = Fixed(sf::Joystick::getAxisPosition(id, sf::Joystick::PovY));
			m_joyInput.m_states.emplace_back(std::move(state));
		}
	}
}
} // namespace LittleEngine
