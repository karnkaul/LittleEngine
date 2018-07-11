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
	ControllerComponent::ControllerComponent(Actor & actor) : Component(actor, "ControllerComponent"), inputHandler(actor.GetActiveLevel().GetInputHandler()) {
		tokens.push_back(inputHandler.Register(GameInput::Left, std::bind(&ControllerComponent::OnLeft, this), OnKey::Held));
		tokens.push_back(inputHandler.Register(GameInput::Right, std::bind(&ControllerComponent::OnRight, this), OnKey::Held));
		tokens.push_back(inputHandler.Register(GameInput::Up, std::bind(&ControllerComponent::OnUp, this), OnKey::Held));
		tokens.push_back(inputHandler.Register(GameInput::Down, std::bind(&ControllerComponent::OnDown, this), OnKey::Held));
		
		// Tests
		tokens.push_back(inputHandler.Register(GameInput::Left, &Test, OnKey::Released));
		tokens.push_back(inputHandler.Register(GameInput::Left, &Test2, OnKey::Released, true));
	}

	ControllerComponent::~ControllerComponent() {
		tokens.clear();
	}
	
	bool _deletedToken = false;

	void ControllerComponent::Tick(Fixed deltaTime) {
		prevDeltaTime = deltaTime;
		Actor& actor = GetActor();

		const World& world = actor.GetActiveLevel().GetWorld();
		const Level& level = actor.GetActiveLevel();
		Vector2 worldX = world.GetScreenBoundsX();
		Vector2 worldY = world.GetScreenBoundsY();
		Vector2 padding = actor.GetComponent<RenderComponent>()->GetWorldBounds(world) * Fixed(1, 2);
		ClampPosition(actor.GetTransform()->localPosition, worldX, worldY, padding);

		// TESTS
		if (actor.GetActiveLevel().LevelTimeMilliSeconds() > 3000 && !_deletedToken) {
			_deletedToken = true;
			tokens.pop_back();
		}
	}

	void ControllerComponent::OnLeft() {
		if (inputHandler.IsKeyPressed(GameInput::LB)) {
			GetActor().GetTransform()->Rotate(prevDeltaTime / 3);
		}
		else {
			GetActor().GetTransform()->localPosition.x -= prevDeltaTime;
		}
	}

	void ControllerComponent::OnRight() {
		if (inputHandler.IsKeyPressed(GameInput::LB)) {
			GetActor().GetTransform()->Rotate(-prevDeltaTime / 3);
		}
		else {
			GetActor().GetTransform()->localPosition.x += prevDeltaTime;
		}
	}

	void ControllerComponent::OnUp() {
		GetActor().GetTransform()->localPosition.y += prevDeltaTime;
	}

	void ControllerComponent::OnDown() {
		GetActor().GetTransform()->localPosition.y -= prevDeltaTime;
	}
}
