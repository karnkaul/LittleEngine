#pragma once
#include <vector>
#include "SFMLInterface/Input.h"

namespace Game {
	enum class GameCommand {
		Invalid,
		MoveLeft,
		MoveRight,
		MoveUp,
		MoveDown,
		RotateLeft,
		RotateRight,
		Enter,
		Exit,
		Fire,
		Select
	};

	class Gamepad {
	public:
		GameCommand Convert(const KeyState& input) const;
		void Bind(KeyCode keyCode, KeyMod keyMod, GameCommand key);

	private:
		struct RawInput {
			KeyCode keyCode;
			KeyMod keyMod;

			RawInput(KeyCode keyCode, KeyMod keyMod);
			bool operator==(const RawInput& rhs) const;
			bool operator!=(const RawInput& rhs) const;
		};

		struct GameInput {
			GameInput(GameCommand key);
			GameInput(GameCommand key, KeyCode keyCode, KeyMod keyMode = KeyMod::Default);
			void Bind(KeyCode keyCode, KeyMod keyMod = KeyMod::Default);
			bool IsMapped(KeyCode keyCode, KeyMod keyMod) const;

			GameCommand key;
			std::vector<RawInput> rawInputs;
		};

		std::vector<GameInput> bindings;
	};
}
