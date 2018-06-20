#include "stdafx.h"
#include "ControllerComponent.h"
#include <functional>
#include "Component.h"
#include "Entities/Actor.h"
#include "Engine/Logger/Logger.h"
#include "Engine/Input/InputHandler.h"
#include "Engine/World.h"
#include "SFMLInterface/Rendering/Renderer.h"
#include "Components/RenderComponent.h"
#include "Utils/Transform.h"
#include "SFMLInterface/Input.h"
#include "Levels/Level.h"

void Test() {
	Game::Logger::Log("Another left detected!");
}

void Test2() {
	Game::Logger::Log("Consuming left detected! (no other Lefts should be triggered)");
}

void ClampPosition(Vector2& position, const Vector2& worldBoundsX, const Vector2& worldBoundsY, const Vector2& padding) {
	if ((position.x - padding.x) < worldBoundsX.x) {
		position.x = worldBoundsX.x + padding.x;
	}
	if ((position.x + padding.x) > worldBoundsX.y) {
		position.x = worldBoundsX.y - padding.x;
	}
	if ((position.y - padding.y) < worldBoundsY.x) {
		position.y = worldBoundsY.x + padding.y;
	}
	if ((position.y + padding.y) > worldBoundsY.y) {
		position.y = worldBoundsY.y - padding.y;
	}
}

namespace Game {
	ControllerComponent::ControllerComponent(Actor & actor) : Component(actor, "ControllerComponent") {
		InputHandler& inputHandler = actor.GetActiveLevel().GetInputHandler();
		tokens.push_back(inputHandler.Register(std::bind(&ControllerComponent::OnMoveLeft, this), GameInput::Left));
		//tokens.push_back(inputHandler.Register(std::bind(&ControllerComponent::OnRotateLeft, this), GameCommand::RotateLeft));
		tokens.push_back(inputHandler.Register(std::bind(&ControllerComponent::OnMoveRight, this), GameInput::Right));
		//tokens.push_back(inputHandler.Register(std::bind(&ControllerComponent::OnRotateRight, this), GameCommand::RotateRight));
		tokens.push_back(inputHandler.Register(std::bind(&ControllerComponent::OnMoveUp, this), GameInput::Up));
		tokens.push_back(inputHandler.Register(std::bind(&ControllerComponent::OnMoveDown, this), GameInput::Down));
		
		// Tests
		tokens.push_back(inputHandler.Register(&Test, GameInput::Left));
		tokens.push_back(inputHandler.Register(&Test2, GameInput::Left, true));
	}

	ControllerComponent::~ControllerComponent() {
		tokens.clear();
	}
	
	bool _deletedToken = false;

	void ControllerComponent::Tick(Fixed deltaTime) {
		prevDeltaTime = deltaTime;
		Actor& actor = GetActor();

		const World& world = actor.GetActiveLevel().GetWorld();
		Vector2 worldX = world.GetWorldBoundsX();
		Vector2 worldY = world.GetWorldBoundsY();
		Vector2 padding = actor.GetComponent<RenderComponent>()->GetRenderer()->GetWorldBounds(world) * Fixed(1, 2);
		ClampPosition(actor.GetTransform()->localPosition, worldX, worldY, padding);

		// TESTS
		if (actor.GetActiveLevel().LevelTimeMilliSeconds() > 3000 && !_deletedToken) {
			_deletedToken = true;
			tokens.pop_back();
		}
	}

	void ControllerComponent::OnMoveLeft() {
		GetActor().GetTransform()->localPosition.x -= prevDeltaTime;
	}

	void ControllerComponent::OnRotateLeft() {
		GetActor().GetTransform()->Rotate(prevDeltaTime / 3);
	}

	void ControllerComponent::OnMoveRight() {
		GetActor().GetTransform()->localPosition.x += prevDeltaTime;
	}

	void ControllerComponent::OnRotateRight() {
		GetActor().GetTransform()->Rotate(-prevDeltaTime / 3);
	}

	void ControllerComponent::OnMoveUp() {
		GetActor().GetTransform()->localPosition.y += prevDeltaTime;
	}

	void ControllerComponent::OnMoveDown() {
		GetActor().GetTransform()->localPosition.y -= prevDeltaTime;
	}
}
