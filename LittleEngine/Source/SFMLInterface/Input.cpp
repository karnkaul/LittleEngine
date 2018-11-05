#include "le_stdafx.h"
#include "Input.h"

namespace LittleEngine {
	const KeyMod KeyMod::Default = KeyMod();

	KeyCode Convert(const sf::Keyboard::Key& code) {
		switch (code) {
		case sf::Keyboard::Left:
			return KeyCode::Left;
		case sf::Keyboard::Right:
			return KeyCode::Right;
		case sf::Keyboard::Up:
			return KeyCode::Up;
		case sf::Keyboard::Down:
			return KeyCode::Down;
		case sf::Keyboard::W:
			return KeyCode::W;
		case sf::Keyboard::A:
			return KeyCode::A;
		case sf::Keyboard::S:
			return KeyCode::S;
		case sf::Keyboard::D:
			return KeyCode::D;
		case sf::Keyboard::E:
			return KeyCode::E;
		case sf::Keyboard::R:
			return KeyCode::R;
		case sf::Keyboard::F:
			return KeyCode::F;
		case sf::Keyboard::Space:
			return KeyCode::Space;
		case sf::Keyboard::Enter:
			return KeyCode::Enter;
		case sf::Keyboard::Escape:
			return KeyCode::Escape;
		case sf::Keyboard::Tab:
			return KeyCode::Tab;
		case sf::Keyboard::LControl:
		case sf::Keyboard::RControl:
			return KeyCode::Control;
		case sf::Keyboard::LShift:
		case sf::Keyboard::RShift:
			return KeyCode::Shift;
		case sf::Keyboard::LAlt:
		case sf::Keyboard::RAlt:
			return KeyCode::Alt;
		case sf::Keyboard::Tilde:
			return KeyCode::Backtick;
		case sf::Keyboard::Backspace:
			return KeyCode::Backspace;
		case sf::Keyboard::F1:
			return KeyCode::F1;
		case sf::Keyboard::F5:
			return KeyCode::F5;
		case sf::Keyboard::F8:
			return KeyCode::F8;
		case sf::Keyboard::F9:
			return KeyCode::F9;
		case sf::Keyboard::F10:
			return KeyCode::F10;
		case sf::Keyboard::F12:
			return KeyCode::F12;
		}
		return KeyCode::Invalid;
	}

	KeyState& Input::GetOrCreateKeyState(KeyCode code) {
		for (auto & keyState : keyStates) {
			if (keyState.GetKeyCode() == code) {
				return keyState;
			}
		}
		keyStates.emplace_back(code);
		return keyStates[keyStates.size() - 1];
	}

	static void HandleRawInput(RawTextInput& rawTextInput, int unicode) {
		if (unicode < 5 || unicode >= 128) {
			return;
		}

		switch (unicode) {
		case 8:
			rawTextInput.special = RawTextInputType::Backspace;
			return;

		case 9:
			rawTextInput.special = RawTextInputType::Tab;
			return;

		case 13:
			rawTextInput.special = RawTextInputType::Enter;
			return;

		case 27:
			rawTextInput.special = RawTextInputType::Escape;
			return;
		}

		rawTextInput.text += static_cast<char>(unicode);
	}

	Input::Input() {
		// Pre-defined keys
		keyStates.emplace_back(KeyCode::Left, "Left");
		keyStates.emplace_back(KeyCode::Right, "Right");
		keyStates.emplace_back(KeyCode::Up, "Up");
		keyStates.emplace_back(KeyCode::Down, "Down");
		keyStates.emplace_back(KeyCode::W, "W");
		keyStates.emplace_back(KeyCode::A, "A");
		keyStates.emplace_back(KeyCode::S, "S");
		keyStates.emplace_back(KeyCode::D, "D");
		keyStates.emplace_back(KeyCode::Space, "Space");
		keyStates.emplace_back(KeyCode::Enter, "Enter");
		keyStates.emplace_back(KeyCode::Escape, "Escape");
		keyStates.emplace_back(KeyCode::Tab, "Tab");
		keyStates.emplace_back(KeyCode::Backtick, "Backtick");
		keyStates.emplace_back(KeyCode::Backspace, "Backspace");
	}

	bool Input::IsKeyPressed(KeyCode code) const {
		for (const auto& iter : keyStates) {
			if (iter.GetKeyCode() == code) {
				return iter.bPressed;
			}
		}
		return false;
	}

	const KeyState Input::GetKeyState(KeyCode code) const {
		for (const auto& iter : keyStates) {
			if (iter.GetKeyCode() == code) {
				return iter;
			}
		}
		return KeyState(code);
	}

	const std::vector<KeyState> Input::GetPressed() const {
		std::vector<KeyState> pressed;
		for (const auto& iter : keyStates) {
			if (iter.bPressed) {
				pressed.emplace_back(iter);
			}
		}
		return pressed;
	}

	const RawTextInput & Input::GetRawSFMLInput() const {
		return rawTextInput;
	}

	void Input::OnKeyDown(const sf::Event::KeyEvent& key) {
		bool bNewKeyCode = true;
		KeyState& toModify = GetOrCreateKeyState(Convert(key.code));
		toModify.bPressed = true;
	}

	void Input::OnKeyUp(const sf::Event::KeyEvent& key) {
		bool bNewKeyCode = true;
		KeyState& toModify = GetOrCreateKeyState(Convert(key.code));
		toModify.bPressed = false;
	}

	void Input::ResetKeyStates() {
		for (auto& keyState : keyStates) {
			keyState.bPressed = false;
		}
	}

	void Input::ClearRawInput() {
		rawTextInput.text.clear();
		rawTextInput.special = RawTextInputType::None;
	}

	void Input::OnRawInput(int unicode) {
		HandleRawInput(rawTextInput, unicode);
	}

	void Input::OnRawSpecialInput(sf::Keyboard::Key key) {
		switch (key) {
		case sf::Keyboard::Up:
			rawTextInput.special = RawTextInputType::Up;
			break;

		case sf::Keyboard::Down:
			rawTextInput.special = RawTextInputType::Down;
			break;

		case sf::Keyboard::Left:
			rawTextInput.special = RawTextInputType::Left;
			break;

		case sf::Keyboard::Right:
			rawTextInput.special = RawTextInputType::Right;
			break;

		default:
			rawTextInput.special = RawTextInputType::None;
		}
	}
}
