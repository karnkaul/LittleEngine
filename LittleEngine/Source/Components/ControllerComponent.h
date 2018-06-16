#pragma once
#include <vector>
#include "Component.h"
#include "Utils/Delegate.hpp"

namespace Game {
	class Actor;
	struct KeyState;
	
	class ControllerComponent : public Component {
	public:
		ControllerComponent(Actor& actor);
		virtual ~ControllerComponent();
		virtual void Tick(Fixed deltaTime) override;

	private:
		std::vector<Delegate<const KeyState&>::Token> tokens;
		Fixed prevDeltaTime = 0;

		void OnLeftPressed(const KeyState& keyState);
		void OnRightPressed(const KeyState& keyState);
		void OnUpPressed(const KeyState& keyState);
		void OnDownPressed(const KeyState& keyState);
	};
}
