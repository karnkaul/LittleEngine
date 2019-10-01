#include "Core/Logger.h"
#include "InputStateMachine.h"
#include "SFMLAPI/System/SFTypes.h"

namespace LittleEngine
{
namespace
{
const Dual<u8> VALID_RANGE = {5, 127};
UMap<u32, KeyCode> ignoreMap = {
	{8, KeyCode::Backspace}, {9, KeyCode::Tab}, {13, KeyCode::Enter}, {27, KeyCode::Escape}, {15, KeyCode::LShift}, {16, KeyCode::RShift},
};
} // namespace

InputStateMachine::InputStateMachine()
{
	// Pre-defined keys
	m_keyStateMap = {
		{KeyCode::Left, {KeyCode::Left, "Left"}},
		{KeyCode::Right, {KeyCode::Right, "Right"}},
		{KeyCode::Up, {KeyCode::Up, "Up"}},
		{KeyCode::Down, {KeyCode::Down, "Down"}},
		{KeyCode::W, {KeyCode::W, "W"}},
		{KeyCode::A, {KeyCode::A, "A"}},
		{KeyCode::S, {KeyCode::S, "S"}},
		{KeyCode::D, {KeyCode::D, "D"}},
		{KeyCode::Space, {KeyCode::Space, "Space"}},
		{KeyCode::Enter, {KeyCode::Enter, "Enter"}},
		{KeyCode::Escape, {KeyCode::Escape, "Escape"}},
		{KeyCode::Tab, {KeyCode::Tab, "Tab"}},
		{KeyCode::Tilde, {KeyCode::Tilde, "Backtick"}},
		{KeyCode::Backspace, {KeyCode::Backspace, "Backspace"}},
		{KeyCode::Delete, {KeyCode::Delete, "Delete"}},

		{KeyType::MOUSE_BTN_0, {KeyType::MOUSE_BTN_0, "Mouse0"}},
		{KeyType::MOUSE_BTN_1, {KeyType::MOUSE_BTN_1, "Mouse1"}},
		{KeyType::MOUSE_BTN_2, {KeyType::MOUSE_BTN_2, "Mouse2"}},
		{KeyType::MOUSE_BTN_3, {KeyType::MOUSE_BTN_3, "Mouse3"}},
		{KeyType::MOUSE_BTN_4, {KeyType::MOUSE_BTN_4, "Mouse4"}},

		{KeyType::JOY_BTN_0, {KeyType::JOY_BTN_0, "Joy0"}},
		{KeyType::JOY_BTN_1, {KeyType::JOY_BTN_1, "Joy1"}},
		{KeyType::JOY_BTN_2, {KeyType::JOY_BTN_2, "Joy2"}},
		{KeyType::JOY_BTN_3, {KeyType::JOY_BTN_3, "Joy3"}},
		{KeyType::JOY_BTN_4, {KeyType::JOY_BTN_4, "Joy4"}},
		{KeyType::JOY_BTN_5, {KeyType::JOY_BTN_5, "Joy5"}},
		{KeyType::JOY_BTN_6, {KeyType::JOY_BTN_6, "Joy6"}},
		{KeyType::JOY_BTN_7, {KeyType::JOY_BTN_7, "Joy7"}},
	};
	m_pointerInput.bInViewport = true;
}

InputStateMachine::~InputStateMachine() = default;

void InputStateMachine::SetInputMapping(InputMap inputMap)
{
	m_inputMap = std::move(inputMap);
}

#define LOG_A_OUTPUT 0
InputDataFrame InputStateMachine::Dataframe() const
{
	InputDataFrame frame;
	frame.textInput = m_textInput;
	for (const auto& kvp : m_keyStateMap)
	bool bApressed = false;
#endif
	for (const auto& kvp : m_keyStateMap)
	{
		const auto& keyState = kvp.second;
		if (keyState.bPressed)
		{
			frame.pressed.push_back(keyState);
#if LOG_A_OUTPUT
			bApressed |= ToS32(keyState.GetKeyType()) == ToS32(KeyCode::A);
#endif
		}
	}
#if LOG_A_OUTPUT
	LOG_D("A pressed: %d", bApressed);
#endif
	frame.mouseInput = m_pointerInput;
	frame.joyInput = m_joyInput;
	return frame;
}

KeyState& InputStateMachine::GetOrCreateKeyState(KeyType key)
{
	if (m_keyStateMap.find(key) == m_keyStateMap.end())
	{
		m_keyStateMap.emplace(key, KeyState(key));
	}
	return m_keyStateMap.at(key);
}

void InputStateMachine::ModifyKeyState(KeyType key, bool bPressed)
{
	if (key != KeyType::KEYTYPE_INVALID)
	{
		KeyState& toModify = GetOrCreateKeyState(key);
		toModify.bPressed = bPressed;
	}
}

void InputStateMachine::OnKeyDown(const sf::Event::KeyEvent& key)
{
	KeyType keyType = static_cast<KeyType>(key.code);
	ModifyKeyState(keyType, true);
	if (keyType != KeyType::KEYTYPE_INVALID)
	{
		m_textInput.metaText.emplace(keyType);
	}
}

void InputStateMachine::OnKeyUp(const sf::Event::KeyEvent& key)
{
	ModifyKeyState(static_cast<KeyType>(key.code), false);
}

void InputStateMachine::OnMouseDown(const sf::Event::MouseButtonEvent& button)
{
	ModifyKeyState(Cast(button.button), true);
}

void InputStateMachine::OnMouseUp(const sf::Event::MouseButtonEvent& button)
{
	ModifyKeyState(Cast(button.button), false);
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
	for (auto& kvp : m_keyStateMap)
	{
		kvp.second.bPressed = false;
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
	if (unicode > VALID_RANGE.first && unicode < VALID_RANGE.second)
	{
		auto iter = ignoreMap.find(unicode);
		if (iter == ignoreMap.end())
		{
			m_textInput.text += static_cast<char>(unicode);
		}
		else
		{
			m_textInput.metaText.emplace(static_cast<KeyType>(iter->second));
		}
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
				if (key != KeyType::KEYTYPE_INVALID)
				{
					KeyState& toModify = GetOrCreateKeyState(key);
					toModify.bPressed = sf::Joystick::isButtonPressed(sfJoystickID, sfButtonID);
					state.pressed.push_back(static_cast<KeyType>(key));
				}
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
