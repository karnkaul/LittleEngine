#include "stdafx.h"
#include "Gamepad.h"
#include "Utils/Utils.h"

namespace Game {
	Gamepad::RawInput::RawInput(KeyCode keyCode)
		: keyCode(keyCode) {
	}

	bool Gamepad::RawInput::operator==(const RawInput & rhs) const {
		return keyCode == rhs.keyCode;
	}

	bool Gamepad::RawInput::operator!=(const RawInput & rhs) const {
		return !(*this == rhs);
	}

	Gamepad::InputMapping::InputMapping(GameInput key) : key(key) {
	}

	Gamepad::InputMapping::InputMapping(GameInput key, KeyCode keyCode)
		: key(key) {
		Bind(keyCode);
	}

	void Gamepad::InputMapping::Bind(KeyCode keyCode) {
		rawInputs.emplace_back(keyCode);
	}

	bool Gamepad::InputMapping::IsMapped(KeyCode keyCode) const {
		RawInput toFind(keyCode);
		auto iter = Utils::VectorSearch(rawInputs, toFind);
		return iter != rawInputs.end();
	}

	GameInput Gamepad::ToGameInput(const KeyState & input) const {
		for (auto& binding : bindings) {
			if (binding.IsMapped(input.GetKeyCode())) {
				return binding.key;
			}
		}
		return GameInput::Invalid;
	}

	void Gamepad::Bind(KeyCode keyCode, GameInput key) {
		for (auto& binding : bindings) {
			if (binding.key == key) {
				if (!binding.IsMapped(keyCode)) {
					binding.Bind(keyCode);
				}
				return;
			}
		}

		bindings.emplace_back(key, keyCode);
	}
}
