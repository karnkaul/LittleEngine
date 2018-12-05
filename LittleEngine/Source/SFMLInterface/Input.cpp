#include "le_stdafx.h"
#include "Input.h"
#include <unordered_map>

namespace LittleEngine {
	namespace {
		std::unordered_map<unsigned int, RawTextInputType> asciiMap = {
			{ 8, RawTextInputType::Backspace },
			{ 9, RawTextInputType::Tab },
			{ 13, RawTextInputType::Enter },
			{ 27, RawTextInputType::Escape }
		};

		KeyCode Convert(const sf::Keyboard::Key& code) {
			switch (code) {
			case sf::Keyboard::Left:		return KeyCode::Left;
			case sf::Keyboard::Right:		return KeyCode::Right;
			case sf::Keyboard::Up:			return KeyCode::Up;
			case sf::Keyboard::Down:		return KeyCode::Down;
			case sf::Keyboard::W:			return KeyCode::W;
			case sf::Keyboard::A:			return KeyCode::A;
			case sf::Keyboard::S:			return KeyCode::S;
			case sf::Keyboard::D:			return KeyCode::D;
			case sf::Keyboard::E:			return KeyCode::E;
			case sf::Keyboard::R:			return KeyCode::R;
			case sf::Keyboard::F:			return KeyCode::F;
			case sf::Keyboard::Space:		return KeyCode::Space;
			case sf::Keyboard::Enter:		return KeyCode::Enter;
			case sf::Keyboard::Escape:		return KeyCode::Escape;
			case sf::Keyboard::Tab:			return KeyCode::Tab;
			case sf::Keyboard::LControl:
			case sf::Keyboard::RControl:	return KeyCode::Control;
			case sf::Keyboard::LShift:
			case sf::Keyboard::RShift:		return KeyCode::Shift;
			case sf::Keyboard::LAlt:
			case sf::Keyboard::RAlt:		return KeyCode::Alt;
			case sf::Keyboard::Tilde:		return KeyCode::Backtick;
			case sf::Keyboard::Backspace:	return KeyCode::Backspace;
			case sf::Keyboard::F1:			return KeyCode::F1;
			case sf::Keyboard::F5:			return KeyCode::F5;
			case sf::Keyboard::F8:			return KeyCode::F8;
			case sf::Keyboard::F9:			return KeyCode::F9;
			case sf::Keyboard::F10:			return KeyCode::F10;
			case sf::Keyboard::F12:			return KeyCode::F12;
			default:						return KeyCode::Invalid;
			}
		}

		void HandleRawInput(RawTextInput& rawTextInput, unsigned int unicode) {
			if (unicode < 5 || unicode >= 128) return;

			auto iter = asciiMap.find(unicode);
			if (iter != asciiMap.end()) rawTextInput.special = iter->second;
			else rawTextInput.text += static_cast<char>(unicode);
		}
	}

	const KeyMod KeyMod::Default = KeyMod();

	KeyState& Input::GetOrCreateKeyState(KeyCode code) {
		for (auto & keyState : m_keyStates) {
			if (keyState.GetKeyCode() == code) {
				return keyState;
			}
		}
		m_keyStates.emplace_back(code);
		return m_keyStates[m_keyStates.size() - 1];
	}

	Input::Input() {
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
		m_keyStates.emplace_back(KeyCode::Backtick, "Backtick");
		m_keyStates.emplace_back(KeyCode::Backspace, "Backspace");
	}

	bool Input::IsKeyPressed(KeyCode code) const {
		for (const auto& iter : m_keyStates) {
			if (iter.GetKeyCode() == code) {
				return iter.bPressed;
			}
		}
		return false;
	}

	const KeyState Input::GetKeyState(KeyCode code) const {
		for (const auto& iter : m_keyStates) {
			if (iter.GetKeyCode() == code) {
				return iter;
			}
		}
		return KeyState(code);
	}

	const std::vector<KeyState> Input::GetPressed() const {
		std::vector<KeyState> pressed;
		for (const auto& iter : m_keyStates) {
			if (iter.bPressed) {
				pressed.emplace_back(iter);
			}
		}
		return pressed;
	}

	const RawTextInput & Input::GetRawSFMLInput() const {
		return m_rawTextInput;
	}

	void Input::OnKeyDown(const sf::Event::KeyEvent& key) {
		KeyState& toModify = GetOrCreateKeyState(Convert(key.code));
		toModify.bPressed = true;
	}

	void Input::OnKeyUp(const sf::Event::KeyEvent& key) {
		KeyState& toModify = GetOrCreateKeyState(Convert(key.code));
		toModify.bPressed = false;
	}

	void Input::ResetKeyStates() {
		for (auto& keyState : m_keyStates) {
			keyState.bPressed = false;
		}
	}

	void Input::ClearRawInput() {
		m_rawTextInput.Reset();
	}

	void Input::OnRawInput(unsigned int unicode) {
		HandleRawInput(m_rawTextInput, unicode);
	}

	void Input::OnRawSpecialInput(sf::Keyboard::Key key) {
		switch (key) {
		case sf::Keyboard::Up:
			m_rawTextInput.special = RawTextInputType::Up;
			break;

		case sf::Keyboard::Down:
			m_rawTextInput.special = RawTextInputType::Down;
			break;

		case sf::Keyboard::Left:
			m_rawTextInput.special = RawTextInputType::Left;
			break;

		case sf::Keyboard::Right:
			m_rawTextInput.special = RawTextInputType::Right;
			break;

		default:
			m_rawTextInput.special = RawTextInputType::None;
		}
	}

	bool RawTextInput::Contains(char c) const {
		size_t idx = text.find(c);
		return idx != std::string::npos;
	}

	void RawTextInput::Reset() {
		text.clear();
		special = RawTextInputType::None;
	}
}
