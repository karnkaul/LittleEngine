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
		W,
		A,
		S,
		D,
		Q,
		E,
		R,
		F,
		Space,
		Enter,
		Escape,
		Tab,
		Control,
		Shift,
		Alt
	};

	// Wrapper struct that holds key state modifiers
	struct KeyMod {
		bool control;
		bool alt;
		bool shift;
		KeyMod() : control(false), alt(false), shift(false) {}
		KeyMod(bool control, bool alt, bool shift) : control(control), alt(alt), shift(shift) {}
		const static KeyMod Default;
	private:
		friend class Input;
		KeyMod(const sf::Event::KeyEvent& event) : control(event.control), alt(event.alt), shift(event.shift) {}
	};

	// Wrapper struct to store state of sf::Event::KeyEvent
	struct KeyState {
		bool pressed;

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
		Input();

		// Call this to check if a Key was pressed in this frame
		bool IsKeyPressed(KeyCode code) const;
		// Get KeyState for this KeyCode on the current frame
		const KeyState GetKeyState(KeyCode code) const;
		// Get all KeyStates pressed in this frame
		const std::vector<KeyState> GetPressed() const;
		
	private:
		std::vector<KeyState> keyStates;

		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;

		KeyState& GetOrCreateKeyState(KeyCode code);
		void OnKeyDown(const sf::Event::KeyEvent& key);
		void OnKeyUp(const sf::Event::KeyEvent& key);
		void ResetKeyStates();

		friend class WindowController;
	};
}
