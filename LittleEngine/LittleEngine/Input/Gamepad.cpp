#include "stdafx.h"
#include "Gamepad.h"
#include "Utils.h"

namespace LittleEngine
{
Gamepad::InputMapping::InputMapping(GameInputType gameInput) : gameInput(gameInput)
{
}

Gamepad::InputMapping::InputMapping(GameInputType gameInput, KeyCode keyCode) : gameInput(gameInput)
{
	keyCodes.emplace_back(keyCode);
}

Gamepad::InputMapping::InputMapping(GameInputType gameInput, InitList<KeyCode> keyCodes)
	: gameInput(gameInput)
{
	for (auto keyCode : keyCodes)
	{
		this->keyCodes.emplace_back(keyCode);
	}
}

bool Gamepad::InputMapping::IsMapped(KeyCode keyCode) const
{
	auto iter = Core::VectorSearch(keyCodes, keyCode);
	return iter != keyCodes.end();
}

GameInputType Gamepad::ToGameInputType(const KeyState& input) const
{
	for (auto& binding : m_bindings)
	{
		if (binding.IsMapped(input.GetKeyCode()))
		{
			return binding.gameInput;
		}
	}
	return GameInputType::Invalid;
}

void Gamepad::Bind(GameInputType gameInput, KeyCode keyCode)
{
	for (auto& binding : m_bindings)
	{
		if (binding.gameInput == gameInput)
		{
			if (!binding.IsMapped(keyCode))
			{
				binding.keyCodes.emplace_back(keyCode);
			}
			return;
		}
	}
	m_bindings.emplace_back(gameInput, keyCode);
}

void Gamepad::Bind(GameInputType gameInput, InitList<KeyCode> keyCodes)
{
	for (auto keyCode : keyCodes)
	{
		Bind(gameInput, keyCode);
	}
}

void Gamepad::Unbind(GameInputType gameInput, KeyCode keyCode)
{
	for (auto& binding : m_bindings)
	{
		if (binding.gameInput == gameInput)
		{
			auto iter = Core::VectorSearch(binding.keyCodes, keyCode);
			if (iter != binding.keyCodes.end())
			{
				binding.keyCodes.erase(iter);
			}
			return;
		}
	}
}

void Gamepad::Unbind(GameInputType gameInput, InitList<KeyCode> keyCodes)
{
	for (auto keyCode : keyCodes)
	{
		Unbind(gameInput, keyCode);
	}
}
} // namespace LittleEngine
