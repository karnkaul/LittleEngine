#include "stdafx.h"
#include "Input.h"

namespace Game {
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
		case sf::Keyboard::Space:
			return KeyCode::Space;
		case sf::Keyboard::Enter:
			return KeyCode::Enter;
		case sf::Keyboard::Escape:
			return KeyCode::Escape;
		case sf::Keyboard::Tab:
			return KeyCode::Tab;
		}
		return KeyCode::Invalid;
	}

	sf::Keyboard::Key Convert(const KeyCode& code) {
		switch (code) {
		case KeyCode::Left:
			return sf::Keyboard::Left;
		case KeyCode::Right:
			return sf::Keyboard::Right;
		case KeyCode::Up:
			return sf::Keyboard::Up;
		case KeyCode::Down:
			return sf::Keyboard::Down;
		case KeyCode::Space:
			return sf::Keyboard::Space;
		case KeyCode::Enter:
			return sf::Keyboard::Enter;
		case KeyCode::Escape:
			return sf::Keyboard::Escape;
		case KeyCode::Tab:
			return sf::Keyboard::Tab;
		}
		return sf::Keyboard::LAlt;
	}

	KeyState* InputHandler::GetOrCreateKeyState(KeyCode code) {
		for (auto & keyState : keyStates) {
			if (keyState.GetKeyCode() == code) {
				return &keyState;
			}
		}
		keyStates.emplace_back(KeyState(code));
		return &keyStates[keyStates.size() - 1];
	}

	bool InputHandler::IsKeyPressed(KeyCode code) const {
		for (const auto& iter : keyStates) {
			if (iter.GetKeyCode() == code) {
				return iter.pressed;
			}
		}
		return false;
	}

	const KeyState InputHandler::GetKeyState(KeyCode code) const {
		for (const auto& iter : keyStates) {
			if (iter.GetKeyCode() == code) {
				return iter;
			}
		}
		return KeyState(code);
	}

	const std::vector<KeyState> InputHandler::GetPressed() const {
		std::vector<KeyState> pressed;
		for (const auto& iter : keyStates) {
			if (iter.pressed) {
				pressed.emplace_back(iter);
			}
		}
		return pressed;
	}

	void InputHandler::OnKeyDown(const sf::Event::KeyEvent& key) {
		bool newKeyCode = true;
		KeyState* toModify = GetOrCreateKeyState(Convert(key.code));
		toModify->modifier = KeyMod(key);
		toModify->pressed = true;
	}

	void InputHandler::OnKeyUp(const sf::Event::KeyEvent& key) {
		bool newKeyCode = true;
		KeyState* toModify = GetOrCreateKeyState(Convert(key.code));
		toModify->modifier = KeyMod();
		toModify->pressed = false;
	}
}
