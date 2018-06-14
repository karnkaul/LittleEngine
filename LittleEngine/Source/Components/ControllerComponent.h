#pragma once
#include "Component.h"

namespace Game {
	class Actor;
	struct KeyState;

	class ControllerComponent : public Component {
	public:
		ControllerComponent(Actor& actor);
		virtual void Tick(Fixed deltaTime) override;

	private:
		void OnKeyPressed(const KeyState & state, Fixed deltaTime);
	};
}
