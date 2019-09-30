#include "Core/Logger.h"
#include "InputStateMachine.h"
#include "SFMLAPI/System/SFTypes.h"

namespace LittleEngine
{
namespace
{
UMap<u32, KeyCode> asciiIgnoreMap = {
	{8, KeyCode::Backspace}, {9, KeyCode::Tab}, {13, KeyCode::Enter}, {27, KeyCode::Escape}, {15, KeyCode::LShift}, {16, KeyCode::RShift},
};

USet<KeyCode> metaTextInputSet = {(KeyCode::Left),	 (KeyCode::Right), (KeyCode::Up), (KeyCode::Down),
								  (KeyCode::Delete), (KeyCode::Home),  (KeyCode::End)};
} // namespace

KeyState& InputStateMachine::GetOrCreateKeyState(KeyType key)
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

InputStateMachine::InputStateMachine()
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
	m_pointerInput.bInViewport = true;
}

InputStateMachine::~InputStateMachine() = default;

bool InputStateMachine::IsKeyPressed(KeyType key) const
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

const KeyState* InputStateMachine::GetKeyState(KeyType key) const
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

void InputStateMachine::SetInputMapping(InputMap inputMap)
{
	m_inputMap = std::move(inputMap);
}

InputDataFrame InputStateMachine::Dataframe() const
{
	InputDataFrame frame;
	frame.textInput = m_textInput;
	for (const auto& iter : m_keyStates)
	{
		if (iter.bPressed)
		{
			frame.pressed.emplace_back(iter);
		}
	}
	frame.mouseInput = m_pointerInput;
	frame.joyInput = m_joyInput;
	return frame;
}

void InputStateMachine::OnKeyDown(const sf::Event::KeyEvent& key)
{
	KeyType keyType = static_cast<KeyType>(key.code);
	KeyState& toModify = GetOrCreateKeyState(keyType);
	toModify.bPressed = true;

	m_textInput.metaText.emplace(keyType);
}

void InputStateMachine::OnKeyUp(const sf::Event::KeyEvent& key)
{
	KeyState& toModify = GetOrCreateKeyState(static_cast<KeyType>(key.code));
	toModify.bPressed = false;
}

void InputStateMachine::OnMouseDown(const sf::Event::MouseButtonEvent& button)
{
	KeyState& toModify = GetOrCreateKeyState(Cast(button.button));
	toModify.bPressed = true;
}

void InputStateMachine::OnMouseUp(const sf::Event::MouseButtonEvent& button)
{
	KeyState& toModify = GetOrCreateKeyState(Cast(button.button));
	toModify.bPressed = false;
}

void InputStateMachine::OnMouseMove(Vector2 worldPosition)
{
	m_pointerInput.worldPosition = worldPosition;
}

void InputStateMachine::OnMouseChange(bool bInViewport)
{
	m_pointerInput.bInViewport = bInViewport;
}

void InputStateMachine::SetMouseWheelScroll(Fixed delta)
{
	m_pointerInput.scrollDelta = delta;
}

void InputStateMachine::ResetKeyStates()
{
	for (auto& keyState : m_keyStates)
	{
		keyState.bPressed = false;
	}
	m_joyInput.m_states.clear();
	m_pointerInput.scrollDelta = Fixed::Zero;
}

void InputStateMachine::ClearTextInput()
{
	m_textInput.Reset();
}

void InputStateMachine::OnTextInput(u32 unicode)
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
	else
	{
		m_textInput.metaText.emplace(static_cast<KeyType>(iter->second));
	}
}

void InputStateMachine::UpdateJoyInput()
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
			auto getAxis = [&](Fixed& var, JoyAxis axis, sf::Joystick::Axis sfAxis, bool bInverse) {
				var = Fixed(sf::Joystick::getAxisPosition(sfJoystickID, sfAxis) / 100);
				if (bInverse)
				{
					var = -var;
				}
				if (Maths::Abs(var) < m_inputMap.DeadZone(axis))
				{
					var = Fixed::Zero;
				}
			};
			getAxis(state.xy.x, JoyAxis::XY, sf::Joystick::X, false);
			getAxis(state.xy.y, JoyAxis::XY, sf::Joystick::Y, true);
			getAxis(state.zr.x, JoyAxis::ZR, sf::Joystick::Z, false);
			getAxis(state.zr.y, JoyAxis::ZR, sf::Joystick::R, false);
			getAxis(state.uv.x, JoyAxis::UV, sf::Joystick::U, false);
			getAxis(state.uv.y, JoyAxis::UV, sf::Joystick::V, true);
			getAxis(state.pov.x, JoyAxis::PoV, sf::Joystick::PovX, false);
			getAxis(state.pov.y, JoyAxis::PoV, sf::Joystick::PovY, false);
			m_joyInput.m_states.emplace_back(std::move(state));
		}
	}
}
} // namespace LittleEngine
