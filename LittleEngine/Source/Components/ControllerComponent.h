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
		void OnLeftPressed(KeyState keyState);
		void OnKeyPressed(const KeyState & state, Fixed deltaTime);
	};
}
