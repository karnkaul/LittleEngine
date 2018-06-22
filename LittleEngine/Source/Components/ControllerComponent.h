#pragma once
#include <vector>
#include "Component.h"
#include "Utils/Delegate.hpp"
#include "Engine/Input/InputHandler.h"

namespace Game {
	class Actor;
	struct KeyState;
	
	class ControllerComponent : public Component {
	public:
		ControllerComponent(Actor& actor);
		virtual ~ControllerComponent();
		virtual void Tick(Fixed deltaTime) override;

	private:
		InputHandler& inputHandler;
		std::vector<OnInput::Token> tokens;
		Fixed prevDeltaTime = 0;

		void OnLeft();
		void OnRight();
		void OnUp();
		void OnDown();
	};
}
