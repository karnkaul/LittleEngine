#pragma once
#include <unordered_set>
#include "Core/CoreTypes.h"
#include "Core/LECoreGame/LECoreUtils/Utils.h"
#include "InputMappings.h"

namespace LittleEngine
{
struct KeyState
{
private:
	s32 keyType;
	std::string_view name;

public:
	bool bPressed;

public:
	KeyState(s32 keyType, std::string_view name = "Unknown");

	KeyType GetKeyType() const;
	std::string_view Name() const;
};

struct TextInput
{
	std::string text;
	std::unordered_set<KeyType> metaText;

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
	std::vector<KeyType> pressed;
};

struct JoyInput
{
	std::vector<JoyState> m_states;
};

struct InputDataFrame
{
	std::vector<KeyState> pressed;
	TextInput textInput;
	MouseInput mouseInput;
	JoyInput joyInput;

	static std::string Clipboard();
};
} // namespace LittleEngine
