#pragma once
#include "SFMLAPI/Input/SFInputStateMachine.h"

namespace LittleEngine
{
// \brief Virtual gamepad input types
enum class GameInputType
{
	Invalid,
	Left,
	Right,
	Up,
	Down,
	Enter,
	Back,
	Select,
	X,
	Y,
	LB,
	RB,

	// Debug
	Debug0,
	Debug1,
	Debug2,
	Debug3,
};

// \brief Virtual gamepad: binds KeyCodes to GameInput
class Gamepad
{
private:
	struct InputMapping
	{
		GameInputType gameInput;
		Vector<KeyCode> keyCodes;

		InputMapping(GameInputType key);
		InputMapping(GameInputType key, KeyCode keyCode);
		InputMapping(GameInputType key, InitList<KeyCode> keyCodes);

		bool IsMapped(KeyCode keyCode) const;
	};

private:
	Vector<InputMapping> m_bindings;

public:
	GameInputType ToGameInputType(const KeyState& input) const;
	void Bind(GameInputType gameInput, KeyCode keyCode);
	void Bind(GameInputType gameInput, InitList<KeyCode> keyCodes);
	void Unbind(GameInputType gameInput, KeyCode keyCode);
	void Unbind(GameInputType gameInput, InitList<KeyCode> keyCodes);
};
} // namespace LittleEngine
