#pragma once
#include <vector>
#include "CoreTypes.h"
#include "SFML/Window.hpp"

namespace LittleEngine
{
using KeyCode = sf::Keyboard::Key;

// \brief Wrapper struct that holds key state modifiers
struct KeyMod
{
	static const KeyMod Default;
	bool bControl;
	bool bAlt;
	bool bShift;

	KeyMod();
	KeyMod(bool bControl, bool bAlt, bool bShift);

private:
	KeyMod(const sf::Event::KeyEvent& event);
	friend class InputHandler;
};

// \brief Wrapper struct to store state of sf::Event::KeyEvent
struct KeyState
{
private:
	String name;
	KeyCode keyCode;

public:
	bool bPressed;

public:
	KeyState(KeyCode keyCode, const String& name = "Unknown");

	KeyCode GetKeyCode() const;
	const String& GetName() const;
};

// \brief Enum to detect special keyboard input (mainly for Console etc)
enum class SpecialInputType
{
	Tab,
	Enter,
	Backspace,
	Escape,
	Up,
	Down,
	Left,
	Right,
	Shift,
	PageUp,
	PageDown,
};

// \brief Struct to hold synchronous input text as a string
struct TextInput
{
	String text;
	Vector<SpecialInputType> specials;

	bool Contains(char c) const;
	bool Contains(SpecialInputType special) const;
	void Reset();
};

struct SFInputDataFrame
{
	Vector<KeyState> pressed;
	TextInput textInput;
};

// \brief Concrete class that a Graphics can update KeyStates to every frame
class SFInputStateMachine
{
private:
	TextInput m_textInput;
	Vector<KeyState> m_keyStates;

public:
	SFInputStateMachine();

	// Call this to check if a Key was pressed in this frame
	bool IsKeyPressed(KeyCode code) const;
	// Get KeyState for this KeyCode on the current frame
	const KeyState GetKeyState(KeyCode code) const;
	const SFInputDataFrame GetFrameInputData() const;

private:
	KeyState& GetOrCreateKeyState(KeyCode code);

	void OnKeyDown(const sf::Event::KeyEvent& key);
	void OnKeyUp(const sf::Event::KeyEvent& key);
	void ResetKeyStates();
	void ClearTextInput();
	void OnTextInput(u32 unicode);

	void StoreNonASCIISpecialInput(KeyCode key);

	friend class SFEventHandler;
};
} // namespace LittleEngine
