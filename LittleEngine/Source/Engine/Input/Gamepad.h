#pragma once
#include <vector>
#include "SFMLInterface/Input.h"

namespace LittleEngine {
	// \brief Virtual gamepad input types
	enum class GameInput {
		Invalid,
		Left,
		Right,
		Up,
		Down,
		Enter,
		Return,
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
	class Gamepad {
	private:
		struct RawInput {
			KeyCode keyCode;

			RawInput(KeyCode keyCode);
			bool operator==(const RawInput& rhs) const;
			bool operator!=(const RawInput& rhs) const;
		};

		struct InputMapping {
			GameInput key;
			std::vector<RawInput> rawInputs;

			InputMapping(GameInput key);
			InputMapping(GameInput key, KeyCode keyCode);
			
			void Bind(KeyCode keyCode);
			bool IsMapped(KeyCode keyCode) const;
		};

	private:
		std::vector<InputMapping> m_bindings;

	public:
		GameInput ToGameInput(const KeyState& input) const;
		void Bind(KeyCode keyCode, GameInput key);
	};
}
