#pragma once
#include <vector>
#include "SFML/Window.hpp"

namespace LittleEngine {
	// \brief Wrapper enum for sf::KeyBoard::Key
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

	// \brief Wrapper struct that holds key state modifiers
	struct KeyMod {
		bool control;
		bool alt;
		bool shift;
		
		const static KeyMod Default;
		
		KeyMod() : control(false), alt(false), shift(false) {}
		KeyMod(bool control, bool alt, bool shift) : control(control), alt(alt), shift(shift) {}
		
	private:
		KeyMod(const sf::Event::KeyEvent& event) : control(event.control), alt(event.alt), shift(event.shift) {}
		friend class Input;
	};

	// \brief Wrapper struct to store state of sf::Event::KeyEvent
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

	// \brief Concrete class that a WindowController can update KeyStates to every frame
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

		// For WindowController
		void OnKeyDown(const sf::Event::KeyEvent& key);
		// For WindowController
		void OnKeyUp(const sf::Event::KeyEvent& key);
		// For WindowController
		void ResetKeyStates();

		friend class WindowController;
	};
}
