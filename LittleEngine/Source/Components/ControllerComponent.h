#pragma once
#include <vector>
#include "Component.h"
#include "Delegate.hpp"
#include "Vector2.h"
#include "Engine/Input/InputHandler.h"

namespace LittleEngine {
	class Actor;
	struct KeyState;

	using Vector2 = GameUtils::Vector2;
	
	// \brief Player Controller Component
	class ControllerComponent : public Component {
	public:
		ControllerComponent(Actor& actor);
		ControllerComponent(Actor& owner, const ControllerComponent& prototype);
		virtual ~ControllerComponent();
		
		virtual void Tick(Fixed deltaTime) override;
		virtual Component::Ptr UClone(Actor& owner) const override;

	private:
		class RenderComponent* renderer = nullptr;
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
