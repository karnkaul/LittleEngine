#include "stdafx.h"
#include "ControllerComponent.h"
#include "Component.h"
#include "Entities/Actor.h"
#include "Engine/Logger/Logger.h"
#include "Utils/Transform.h"
#include "SFMLInterface/InputHandler.h"
#include "Levels/Level.h"

namespace Game {
	ControllerComponent::ControllerComponent(Actor & actor) : Component(actor, "ControllerComponent") {
	}
	
	void ControllerComponent::Tick(Fixed deltaTime) {
		std::vector<KeyState> keys = GetActor().GetActiveLevel().GetInputHandler().GetPressed();
		for (const auto& key : keys) {
			OnKeyPressed(key, deltaTime);
		}
	}

	void ControllerComponent::OnKeyPressed(const KeyState & state, Fixed deltaTime) {
		bool control = state.modifier.control;
		switch (state.GetKeyCode()) {
		case KeyCode::Left:
			if (control) GetActor().GetTransform()->Rotate(deltaTime / 3);
			else GetActor().GetTransform()->localPosition.x -= deltaTime;
			break;

		case KeyCode::Right:
			if (control) GetActor().GetTransform()->Rotate(-deltaTime / 3);
			else GetActor().GetTransform()->localPosition.x += deltaTime;
			break;

		case KeyCode::Up:
			GetActor().GetTransform()->localPosition.y += deltaTime;
			break;

		case KeyCode::Down:
			GetActor().GetTransform()->localPosition.y -= deltaTime;
			break;

		case KeyCode::Space:
			Logger::Log(*this, "Not implemented");
		}
	}
}
