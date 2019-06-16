#pragma once
#include "SFMLAPI/Input/SFInputStateMachine.h"

namespace LittleEngine
{
// \brief Virtual gamepad: binds KeyCodes to GameInput
//class Gamepad
//{
//private:
//	struct InputMapping
//	{
//		KeyType gameInput;
//		Vec<KeyCode> keyCodes;
//		Vec<u16> joyButtons;
//
//		InputMapping(GameInputType gameInput);
//		InputMapping(GameInputType gameInput, KeyCode keyCode);
//		InputMapping(GameInputType gameInput, InitList<KeyCode> keyCodes);
//		InputMapping(GameInputType gameInput, u16 joyButton);
//
//		bool IsMapped(KeyCode keyCode) const;
//		bool IsMapped(u16 joyButton) const;
//	};
//
//private:
//	Vec<InputMapping> m_bindings;
//
//public:
//	GameInputType ToGameInputType(const KeyState& input) const;
//	GameInputType ToGameInputType(u8 joyButton) const;
//	void Bind(GameInputType gameInput, KeyCode keyCode);
//	void Bind(GameInputType gameInput, InitList<KeyCode> keyCodes);
//	void Bind(GameInputType gameInput, u16 joyButton);
//	void Bind(GameInputType gameInput, InitList<u16> joyButtons);
//	void Unbind(GameInputType gameInput, KeyCode keyCode);
//	void Unbind(GameInputType gameInput, InitList<KeyCode> keyCodes);
//};
} // namespace LittleEngine
