#pragma once
#include <vector>
#include "Component.h"
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
		std::vector<InputHandler::Token> tokens;
		Fixed prevDeltaTime = 0;

		void OnLeftPressed(KeyState keyState);
		void OnRightPressed(KeyState keyState);
		void OnUpPressed(KeyState keyState);
		void OnDownPressed(KeyState keyState);
	};
}
