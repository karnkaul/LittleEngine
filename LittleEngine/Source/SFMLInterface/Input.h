#pragma once
#include <vector>
#include "SFML/Window.hpp"

namespace Game {
	// Wrapper enum for sf::KeyBoard::Key
	enum class KeyCode {
		Invalid,
		Left,
		Right,
		Up,
		Down,
		Space,
		Enter,
		Escape,
		Tab
	};

	// Wrapper struct that holds key state modifiers
	struct KeyMod {
		bool control;
		bool alt;
		bool shift;
		KeyMod() : control(false), alt(false), shift(false) {}
		KeyMod(bool control, bool alt, bool shift) : control(control), alt(alt), shift(shift) {}
	private:
		friend class Input;
		KeyMod(const sf::Event::KeyEvent& event) : control(event.control), alt(event.alt), shift(event.shift) {}
	};

	// Wrapper struct to store state of sf::Event::KeyEvent
	struct KeyState {
		bool pressed;
		KeyMod modifier;

		KeyState(KeyCode keyCode) : keyCode(keyCode), pressed(false), name("Unknown") {}
		KeyState(KeyCode keyCode, const std::string& name) : keyCode(keyCode), pressed(false), name(name) {}
		KeyCode GetKeyCode() const { return keyCode; }
		const std::string& GetName() const { return name; }
	private:
		KeyCode keyCode;
		std::string name;
	};

	// Concrete class that a WindowController can update KeyStates to every frame
	class Input {
	public:
		Input() {
			// Pre-defined keys
			keyStates.emplace_back(KeyState(KeyCode::Left, "Left"));
			keyStates.emplace_back(KeyState(KeyCode::Right, "Right"));
			keyStates.emplace_back(KeyState(KeyCode::Up, "Up"));
			keyStates.emplace_back(KeyState(KeyCode::Down, "Down"));
			keyStates.emplace_back(KeyState(KeyCode::Space, "Space"));
			keyStates.emplace_back(KeyState(KeyCode::Enter, "Enter"));
			keyStates.emplace_back(KeyState(KeyCode::Escape, "Escape"));
			keyStates.emplace_back(KeyState(KeyCode::Tab, "Tab"));
		}

		// Call this to check if a Key was pressed in this frame
		bool IsKeyPressed(KeyCode code) const;
		// Get KeyState for this KeyCode on the current frame
		const KeyState GetKeyState(KeyCode code) const;
		// Get all KeyStates pressed in this frame
		const std::vector<KeyState> GetPressed() const;
		
	private:
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;
		friend class WindowController;
		KeyState* GetOrCreateKeyState(KeyCode code);
		void OnKeyDown(const sf::Event::KeyEvent& key);
		void OnKeyUp(const sf::Event::KeyEvent& key);
		std::vector<KeyState> keyStates;
	};
}
