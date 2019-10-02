#pragma once
#include "SFML/Window/Keyboard.hpp"
#include "Core/CoreTypes.h"
#include "Core/Property.h"

namespace LittleEngine
{
extern const u32 g_MAX_JOYSTICKS;
extern const u32 g_MAX_JOYSTICK_BUTTONS;

using KeyCode = sf::Keyboard::Key;

enum KeyType : s32
{
	KEYTYPE_INVALID = -1,
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

enum class JoyAxis : u8
{
	XY = 0,
	ZR,
	UV,
	PoV,
};

class InputMap
{
private:
	UMap<u32, KeyType> m_keyMap = {{0, JOY_BTN_0}, {1, JOY_BTN_1}, {2, JOY_BTN_2}, {3, JOY_BTN_3},
								   {4, JOY_BTN_4}, {5, JOY_BTN_5}, {6, JOY_BTN_6}, {7, JOY_BTN_7}};
	UMap<JoyAxis, Fixed> m_deadZones = {{JoyAxis::XY, Fixed(15, 100)}, {JoyAxis::ZR, Fixed(10, 100)}};

public:
	KeyType GetKeyType(u32 button) const;
	Fixed DeadZone(JoyAxis axis) const;

	u16 Import(const Core::Property::Persistor& persistor);
};
} // namespace LittleEngine
