#include "stdafx.h"
#include "ControllerComponent.h"
#include <functional>
#include "Component.h"
#include "Entities/Actor.h"
#include "Engine/Logger/Logger.h"
#include "Engine/Input/InputHandler.h"
#include "Utils/Transform.h"
#include "SFMLInterface/Input.h"
#include "Levels/Level.h"

void Test(const Game::KeyState& keyState) {
	Game::Logger::Log("Another left detected!");
}

void Test2(const Game::KeyState& keyState) {
	Game::Logger::Log("Consuming left detected! (no other Lefts should be triggered)");
}

namespace Game {
	ControllerComponent::ControllerComponent(Actor & actor) : Component(actor, "ControllerComponent") {
		InputHandler& inputHandler = actor.GetActiveLevel().GetInputHandler();
		tokens.push_back(inputHandler.Register(std::bind(&ControllerComponent::OnLeftPressed, this, std::placeholders::_1), KeyCode::Left));
		tokens.push_back(inputHandler.Register(&Test, KeyCode::Left, true));
		tokens.push_back(inputHandler.Register(&Test2, KeyCode::Left, true));
	}

	ControllerComponent::~ControllerComponent() {
		tokens.clear();
	}
	
	bool _deletedToken = false;
	void ControllerComponent::Tick(Fixed deltaTime) {
		std::vector<KeyState> keys = GetActor().GetActiveLevel().GetInput().GetPressed();
		for (const auto& key : keys) {
			OnKeyPressed(key, deltaTime);
		}

		if (GetActor().GetActiveLevel().LevelTimeMilliSeconds() > 3000 && !_deletedToken) {
			_deletedToken = true;
			tokens.pop_back();
		}
	}

	void ControllerComponent::OnLeftPressed(KeyState keyState) {
		Logger::Log(*this, "Left detected!");
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
