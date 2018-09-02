#include "stdafx.h"
#include "ControllerComponent.h"
#include <functional>
#include "Component.h"
#include "Entities/Actor.h"
#include "Engine/Logger/Logger.h"
#include "Engine/Input/inputHandler.h"
#include "Engine/World.h"
#include "SFMLInterface/Rendering/Renderer.h"
#include "Components/RenderComponent.h"
#include "Utils/Transform.h"
#include "SFMLInterface/Input.h"
#include "Levels/Level.h"

namespace _ControllerComponent {
	void Test() {
		Game::Logger::Log("Another left detected!");
	}

	void Test2() {
		Game::Logger::Log("Consuming left detected! (no other Lefts should be triggered)");
	}
}

void ClampPosition(Vector2& position, const Rect2& worldBounds, const Vector2& padding) {
	if ((position.x - padding.x) < worldBounds.lower.x) {
		position.x = worldBounds.lower.x + padding.x;
	}
	if ((position.x + padding.x) > worldBounds.upper.x) {
		position.x = worldBounds.upper.x - padding.x;
	}
	if ((position.y - padding.y) < worldBounds.lower.y) {
		position.y = worldBounds.lower.y + padding.y;
	}
	if ((position.y + padding.y) > worldBounds.upper.y) {
		position.y = worldBounds.upper.y - padding.y;
	}
}

namespace Game {
	bool _deletedToken = false;

	ControllerComponent::ControllerComponent(Actor & actor) : Component(actor, "ControllerComponent") {
		this->inputHandler = &actor.GetActiveLevel().GetInputHandler();
		tokens.push_back(inputHandler->Register(GameInput::Left, std::bind(&ControllerComponent::OnLeft, this), OnKey::Held));
		tokens.push_back(inputHandler->Register(GameInput::Right, std::bind(&ControllerComponent::OnRight, this), OnKey::Held));
		tokens.push_back(inputHandler->Register(GameInput::Up, std::bind(&ControllerComponent::OnUp, this), OnKey::Held));
		tokens.push_back(inputHandler->Register(GameInput::Down, std::bind(&ControllerComponent::OnDown, this), OnKey::Held));
		renderer = actor.GetComponent<RenderComponent>();
		
		// Tests
		_deletedToken = false;
		tokens.push_back(inputHandler->Register(GameInput::Left, &_ControllerComponent::Test, OnKey::Released));
		tokens.push_back(inputHandler->Register(GameInput::Left, &_ControllerComponent::Test2, OnKey::Released, true));
	}

	ControllerComponent::~ControllerComponent() {
		tokens.clear();
	}

	void ControllerComponent::Tick(Fixed deltaTime) {
		prevDeltaTime = deltaTime;
		Actor& actor = GetActor();

		const World& world = actor.GetActiveLevel().GetWorld();
		const Level& level = actor.GetActiveLevel();
		Rect2 worldBounds = world.GetScreenBounds();
		Vector2 padding = GetRenderPadding();
		ClampPosition(actor.GetTransform().localPosition, worldBounds, padding);

		// TESTS
		if (actor.GetActiveLevel().LevelTimeMilliSeconds() > 2000 && !_deletedToken) {
			_deletedToken = true;
			tokens.pop_back();
		}
	}

	Vector2 ControllerComponent::GetRenderPadding() {
		if (renderer == nullptr) {
			renderer = GetActor().GetComponent<RenderComponent>();
		}
		if (renderer == nullptr) {
			Logger::Log(*this, "ControllerComponent's owning Actor does not have a RenderComponent", Logger::Severity::Warning);
			return Vector2::Zero;
		}
		return renderer->GetBounds().upper;
	}

	void ControllerComponent::OnLeft() {
		if (inputHandler->IsKeyPressed(GameInput::LB)) {
			GetActor().GetTransform().Rotate(prevDeltaTime / 3);
		}
		else {
			GetActor().GetTransform().localPosition.x -= prevDeltaTime;
		}
	}

	void ControllerComponent::OnRight() {
		if (inputHandler->IsKeyPressed(GameInput::LB)) {
			GetActor().GetTransform().Rotate(-prevDeltaTime / 3);
		}
		else {
			GetActor().GetTransform().localPosition.x += prevDeltaTime;
		}
	}

	void ControllerComponent::OnUp() {
		GetActor().GetTransform().localPosition.y += prevDeltaTime;
	}

	void ControllerComponent::OnDown() {
		GetActor().GetTransform().localPosition.y -= prevDeltaTime;
	}
}
