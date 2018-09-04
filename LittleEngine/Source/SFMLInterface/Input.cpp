#include "stdafx.h"
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
	}

	bool Input::IsKeyPressed(KeyCode code) const {
		for (const auto& iter : keyStates) {
			if (iter.GetKeyCode() == code) {
				return iter.pressed;
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
			if (iter.pressed) {
				pressed.emplace_back(iter);
			}
		}
		return pressed;
	}

	void Input::OnKeyDown(const sf::Event::KeyEvent& key) {
		bool newKeyCode = true;
		KeyState& toModify = GetOrCreateKeyState(Convert(key.code));
		toModify.pressed = true;
	}

	void Input::OnKeyUp(const sf::Event::KeyEvent& key) {
		bool newKeyCode = true;
		KeyState& toModify = GetOrCreateKeyState(Convert(key.code));
		toModify.pressed = false;
	}

	void Input::ResetKeyStates() {
		for (auto& keyState : keyStates) {
			keyState.pressed = false;
		}
	}
}
