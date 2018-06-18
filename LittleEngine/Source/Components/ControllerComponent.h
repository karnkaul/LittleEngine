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
		std::vector<OnInput::Token> tokens;
		Fixed prevDeltaTime = 0;

		void OnMoveLeft();
		void OnRotateLeft();
		void OnMoveRight();
		void OnRotateRight();
		void OnMoveUp();
		void OnMoveDown();
	};
}
