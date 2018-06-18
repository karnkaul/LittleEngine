#include "stdafx.h"
#include "Gamepad.h"

namespace Game {
	Gamepad::RawInput::RawInput(KeyCode keyCode, KeyMod keyMod)
		: keyMod(keyMod), keyCode(keyCode) {
	}

	bool Gamepad::RawInput::operator==(const RawInput & rhs) const {
		return 
			keyMod.alt == rhs.keyMod.alt &&
			keyMod.control == rhs.keyMod.control &&
			keyMod.shift == rhs.keyMod.shift &&
			keyCode == rhs.keyCode;
	}

	bool Gamepad::RawInput::operator!=(const RawInput & rhs) const {
		return !(*this == rhs);
	}

	Gamepad::GameInput::GameInput(GameCommand key) : key(key) {
	}

	Gamepad::GameInput::GameInput(GameCommand key, KeyCode keyCode, KeyMod keyMod)
		: key(key) {
		Bind(keyCode, keyMod);
	}

	void Gamepad::GameInput::Bind(KeyCode keyCode, KeyMod keyMod) {
		rawInputs.emplace_back(keyCode, keyMod);
	}

	bool Gamepad::GameInput::IsMapped(KeyCode keyCode, KeyMod keyMod) const {
		RawInput toFind(keyCode, keyMod);
		auto iter = std::find(rawInputs.begin(), rawInputs.end(), toFind);
		return iter != rawInputs.end();
	}

	GameCommand Gamepad::Convert(const KeyState & input) const {
		for (auto& binding : bindings) {
			if (binding.IsMapped(input.GetKeyCode(), input.modifier)) {
				return binding.key;
			}
		}
		return GameCommand::Invalid;
	}

	void Gamepad::Bind(KeyCode keyCode, KeyMod keyMod, GameCommand key) {
		for (auto& binding : bindings) {
			if (binding.key == key) {
				if (!binding.IsMapped(keyCode, keyMod)) {
					binding.Bind(keyCode, keyMod);
				}
				return;
			}
		}

		bindings.emplace_back(key, keyCode, keyMod);
	}
}
