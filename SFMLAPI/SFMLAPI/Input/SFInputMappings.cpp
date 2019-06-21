#include "stdafx.h"
#include "Core/Logger.h"
#include "Core/Utils.h"
#include "SFInputMappings.h"

namespace LittleEngine
{
const u32 g_MAX_JOYSTICKS = 8;
const u32 g_MAX_JOYSTICK_BUTTONS = 32;

KeyType InputMap::GetKeyType(u32 button) const
{
	auto iter = keyMap.find(button);
	if (iter != keyMap.end())
	{
		return iter->second;
	}
	return static_cast<KeyType>(0);
}

u16 InputMap::Import(const Core::Property::Persistor& persistor)
{
	// Joystick
	String prefix = "JOY_BTN_";
	s32 btnID = JOY_BTN_0;
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
				keyMap[mappedID] = static_cast<KeyType>(btnID);
				++mapped;
				LOG_D("[InputMap] Mapped %s -> (SF Joystick Button ID) %d", key.c_str(), mappedID);
			}
		}
		++btnID;
	}
	return mapped;
}
}