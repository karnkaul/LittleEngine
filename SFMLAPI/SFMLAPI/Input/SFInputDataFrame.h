#pragma once
#include "Core/CoreTypes.h"
#include "SFML/Window.hpp"
#include "Core/Utils.h"

namespace LittleEngine
{
using KeyCode = sf::Keyboard::Key;
extern const u32 g_MAX_JOYSTICKS;

enum KeyType
{
	// 0-499 : SFML

	MOUSE_BTN_0 = 500, // Left
	MOUSE_BTN_1,	   // Right
	MOUSE_BTN_2,	   // Middle
	MOUSE_BTN_3,
	MOUSE_BTN_4,
	JOY_BTN_0, // A
	JOY_BTN_1, // B
	JOY_BTN_2, // X
	JOY_BTN_3, // Y
	JOY_BTN_4, // LB
	JOY_BTN_5, // RB
	JOY_BTN_6, // Select
	JOY_BTN_7  // Start
};

struct KeyState
{
private:
	const char* szName;
	s32 keyType;

public:
	bool bPressed;

public:
	KeyState(s32 keyType, const char* szName = "Unknown");

	KeyType GetKeyType() const;
	const char* GetNameStr() const;
};

struct TextInput
{
	String text;
	Vec<KeyType> keys;
	USet<KeyCode> ignoredChars;

	bool ContainsChar(char c) const;
	bool ContainsKey(s32 keyCode) const;
	bool ContainsIgnored(KeyCode keyCode) const;
	void Reset();
};

struct MouseInput
{
	Vector2 worldPosition;
	Fixed scrollDelta;
};

struct JoyState
{
	s16 id;
	Vector2 xy;
	Vector2 zr;
	Vector2 uv;
	Vector2 pov;
	Vec<KeyType> pressed;
};

struct JoyInput
{
	Vec<JoyState> m_states;
};

struct SFInputDataFrame
{
	Vec<KeyState> pressed;
	TextInput textInput;
	MouseInput mouseInput;
	JoyInput joyInput;

	static String GetClipboard();
};
} // namespace LittleEngine