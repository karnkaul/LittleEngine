#pragma once
#include <vector>
#include "Component.h"
#include "Utils/Delegate.hpp"
#include "Utils/Vector2.h"
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
		std::shared_ptr<class RenderComponent> renderer;
		std::vector<OnInput::Token> tokens;
		Fixed prevDeltaTime = 0;

		Vector2 GetRenderPadding();
		void OnLeft();
		void OnRight();
		void OnUp();
		void OnDown();
	};
}
