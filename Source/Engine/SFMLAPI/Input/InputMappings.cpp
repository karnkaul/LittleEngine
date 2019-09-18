#include "Core/Logger.h"
#include "Core/Utils.h"
#include "InputMappings.h"

namespace LittleEngine
{
const u32 g_MAX_JOYSTICKS = 8;
const u32 g_MAX_JOYSTICK_BUTTONS = 32;

KeyType InputMap::GetKeyType(u32 button) const
{
	auto search = m_keyMap.find(button);
	if (search != m_keyMap.end())
	{
		return search->second;
	}
	return static_cast<KeyType>(0);
}

Fixed InputMap::DeadZone(JoyAxis axis) const
{
	auto search = m_deadZones.find(axis);
	if (search != m_deadZones.end())
	{
		return search->second;
	}
	return Fixed::Zero;
}

u16 InputMap::Import(const Core::Property::Persistor& persistor)
{
	// Joystick Axis Deadzone
	auto getAxisDZ = [&](String axisPrefix, JoyAxis axis) {
		String key = axisPrefix + "_DEAD_ZONE";
		auto pProp = persistor.GetProp(key);
		if (pProp)
		{
			m_deadZones[axis] = Fixed(pProp->ToF32(0.0f));
			LOG_D("[InputMap] [%s] Dead Zone set to [%.2f]", axisPrefix.c_str(), m_deadZones[axis].ToF32());
		}
	};
	getAxisDZ("XY", JoyAxis::XY);
	getAxisDZ("ZR", JoyAxis::ZR);
	getAxisDZ("UV", JoyAxis::UV);

	// Joystick Buttons
	String prefix = "JOY_BTN_";
	s32 btnID = KeyType::JOY_BTN_0;
	u16 mapped = 0;
	for (u32 sfBtnID = 0; sfBtnID < g_MAX_JOYSTICK_BUTTONS; ++sfBtnID)
	{
		String key = prefix + Strings::ToString(sfBtnID);
		const auto pProperty = persistor.GetProp(key);
		if (pProperty)
		{
			s32 mappedID = pProperty->ToS32(-1);
			if (mappedID > -1)
			{
				m_keyMap[static_cast<u32>(mappedID)] = static_cast<KeyType>(btnID);
				++mapped;
				LOG_D("[InputMap] Mapped [%s] -> (SF Joystick Button ID) [%d]", key.c_str(), mappedID);
			}
		}
		++btnID;
	}
	return mapped;
}
} // namespace LittleEngine
