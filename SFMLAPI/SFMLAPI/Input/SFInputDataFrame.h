#pragma once
#include "Core/CoreTypes.h"
#include "SFML/Window.hpp"
#include "Core/Utils.h"

namespace LittleEngine
{
using KeyCode = sf::Keyboard::Key;

// \brief Wrapper struct to store state of sf::Event::KeyEvent
struct KeyState
{
private:
	String name;
	KeyCode keyCode;

public:
	bool bPressed;

public:
	KeyState(KeyCode keyCode, const char* szName = "Unknown");

	KeyCode GetKeyCode() const;
	const char* GetNameStr() const;
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
	Control,
	Alt,
	Insert,
	Delete,
	PageUp,
	PageDown,
	Home,
	End,
};

// \brief Struct to hold synchronous input text as a string
struct TextInput
{
	String text;
	Vec<SpecialInputType> specials;

	bool Contains(char c) const;
	bool Contains(SpecialInputType special) const;
	void Reset();
};

struct MouseInput
{
	Vector2 worldPosition;
	bool bLeftPressed = false;
	bool bRightPressed = false;
};

struct SFInputDataFrame
{
	Vec<KeyState> pressed;
	TextInput textInput;
	MouseInput mouseInput;

	static String GetClipboard();
};
} // namespace LittleEngine