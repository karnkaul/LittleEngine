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
		Vec<KeyCode> keyCodes;

		InputMapping(GameInputType gameInput);
		InputMapping(GameInputType gameInput, KeyCode keyCode);
		InputMapping(GameInputType gameInput, InitList<KeyCode> keyCodes);

		bool IsMapped(KeyCode keyCode) const;
	};

private:
	Vec<InputMapping> m_bindings;

public:
	GameInputType ToGameInputType(const KeyState& input) const;
	void Bind(GameInputType gameInput, KeyCode keyCode);
	void Bind(GameInputType gameInput, InitList<KeyCode> keyCodes);
	void Unbind(GameInputType gameInput, KeyCode keyCode);
	void Unbind(GameInputType gameInput, InitList<KeyCode> keyCodes);
};
} // namespace LittleEngine
