#pragma once
#include "Core/CoreTypes.h"
#include "Core/Utils.h"
#include "InputMappings.h"

namespace LittleEngine
{
struct KeyState
{
private:
	s32 keyType;
	VString name;

public:
	bool bPressed;

public:
	KeyState(s32 keyType, VString name = "Unknown");

	KeyType GetKeyType() const;
	VString Name() const;
};

struct TextInput
{
	String text;
	USet<KeyType> metaText;

	bool ContainsChar(char c) const;
	bool ContainsKey(s32 keyCode) const;
	void Reset();
};

struct MouseInput
{
	Vector2 worldPosition;
	Fixed scrollDelta;
	bool bInViewport = false;
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

struct InputDataFrame
{
	Vec<KeyState> pressed;
	TextInput textInput;
	MouseInput mouseInput;
	JoyInput joyInput;

	static String Clipboard();
};
} // namespace LittleEngine
