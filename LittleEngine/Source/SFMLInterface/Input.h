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
		Alt,
		Backtick,
		Backspace,
		Insert,
		Delete,
		F1,
		F5,
		F8,
		F9,
		F10,
		F12
	};

	// \brief Wrapper struct that holds key state modifiers
	struct KeyMod {
		bool bControl;
		bool bAlt;
		bool bShift;
		
		const static KeyMod Default;
		
		KeyMod() : bControl(false), bAlt(false), bShift(false) {}
		KeyMod(bool bControl, bool bAlt, bool bShift) : bControl(bControl), bAlt(bAlt), bShift(bShift) {}
		
	private:
		KeyMod(const sf::Event::KeyEvent& event) : bControl(event.control), bAlt(event.alt), bShift(event.shift) {}
		friend class Input;
	};

	// \brief Wrapper struct to store state of sf::Event::KeyEvent
	struct KeyState {
	private:
		std::string name;
		KeyCode keyCode;

	public:
		bool bPressed;

		KeyState(KeyCode keyCode) : name("Unknown"), keyCode(keyCode), bPressed(false) {}
		KeyState(KeyCode keyCode, const std::string& name) : name(name), keyCode(keyCode), bPressed(false) {}
		KeyCode GetKeyCode() const { return keyCode; }
		const std::string& GetName() const { return name; }
	};

	// \brief Enum to detect special keyboard input (mainly for Console etc)
	enum class RawTextInputType {
		None,
		Tab,
		Enter,
		Backspace,
		Escape,
		Up,
		Down,
		Left,
		Right
	};

	// \brief Struct to hold synchronous input text as a string
	struct RawTextInput {
		std::string text;
		RawTextInputType special;
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
		// Get synchronised text input (as string)
		const RawTextInput& GetRawSFMLInput() const;
		
	private:
		std::vector<KeyState> keyStates;
		RawTextInput rawTextInput;

		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;

		KeyState& GetOrCreateKeyState(KeyCode code);

		// For WindowController
		void OnKeyDown(const sf::Event::KeyEvent& key);
		// For WindowController
		void OnKeyUp(const sf::Event::KeyEvent& key);
		// For WindowController
		void ResetKeyStates();
		// For WindowController
		void ClearRawInput();
		// For WindowController
		void OnRawInput(int unicode);
		// For WindowController (to capture non-ASCII keycodes, like cursor keys)
		void OnRawSpecialInput(sf::Keyboard::Key key);

		friend class WindowController;
	};
}
