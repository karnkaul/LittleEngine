#pragma once
#include <vector>
#include "Component.h"
#include "Utils/Delegate.hpp"
#include "Utils/Vector2.h"
#include "Engine/Input/InputHandler.h"

namespace Game {
	class Actor;
	struct KeyState;
	
	// \brief Player Controller Component
	class ControllerComponent : public Component {
	public:
		ControllerComponent(Actor& actor);
		virtual ~ControllerComponent();
		virtual void Tick(Fixed deltaTime) override;

	private:
		std::shared_ptr<class RenderComponent> renderer;
		std::vector<OnInput::Token> tokens;
		Fixed prevDeltaTime = 0;
		InputHandler* inputHandler;

		Vector2 GetRenderPadding();
		void OnLeft();
		void OnRight();
		void OnUp();
		void OnDown();
	};
}
