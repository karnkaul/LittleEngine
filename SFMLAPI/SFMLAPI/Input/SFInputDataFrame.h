#pragma once
#include "CoreTypes.h"
#include "SFML/Window.hpp"
#include "Utils.h"

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

struct SFInputDataFrame
{
	Vec<KeyState> pressed;
	TextInput textInput;

	static String GetClipboard();
};
} // namespace LittleEngine