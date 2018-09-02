#include "stdafx.h"
#include <iostream>

#include "Actor.h"
#include "Engine/Logger/Logger.h"
#include "Engine/World.h"
#include "Levels/Level.h"
#include "Levels/LevelManager.h"
#include "SFMLInterface/Assets.h"
#include "SFMLInterface/Rendering/RenderParams.h"
#include "SFMLInterface/Rendering/Renderer.h"
#include "SFMLInterface/Rendering/RenderFactory.h"
#include "SFMLInterface/Rendering/SpriteRenderer.h"
#include "Components/Component.h"
#include "Utils/Utils.h"

namespace Game {
	Actor::Actor(Level& level, std::string name) : Object(name) {
		this->level = &level;
		transform = Transform::Create();
		Logger::Log(*this, "Actor Spawned at " + transform->Position().ToString());
	}

	Actor::~Actor() {
		if (collider != nullptr && GetActiveLevel().GetCollisionManager().Unregister(collider)) {
			collider = nullptr;
		}
		components.clear();
		Logger::Log(*this, "Actor Destroyed");
	}

	void Actor::SetNormalisedPosition(Vector2 localNPosition) {
		// -1 <= x, y <= 1
		localNPosition.x = Maths::Clamp_11(localNPosition.x);
		localNPosition.y = Maths::Clamp_11(localNPosition.y);
		// r` = r * screen.r
		Vector2 screenSize = level->GetWorld().GetScreenBounds().upper;
		Vector2 newPos(localNPosition.x * screenSize.x, localNPosition.y * screenSize.y);
		transform->localPosition = newPos;
	}

	std::string Actor::ToString() const {
		return name + " : " + transform->ToString();
	}

	void Actor::Destruct() {
		// Allow ActiveLevel to collect this Actor
		_destroyed = true;
	}

	void Actor::FixedTick() {
		// Don't do anything if about to be destroyed
		if (_destroyed) {
			return;
		}
		// FixedTick each component
		for (const auto& component : components) {
			if (component != nullptr && component->enabled) {
				component->FixedTick();
			}
		}
		// FixedTick collider if any
		if (collider != nullptr) {
			collider->FixedTick();
		}
	}

	void Actor::Tick(Fixed deltaTime) {
		// Don't do anything if about to be destroyed
		if (_destroyed) {
			return;
		}
		// Tick each enabled component
		for (const auto& component : components) {
			if (component->enabled) {
				component->Tick(deltaTime);
			}
		}
		// Tick collider if any
		if (collider != nullptr) {
			collider->Tick(deltaTime);
		}
	}

	void Actor::Render(RenderParams& params) {
		// Don't add to render buffer if about to be destroyed
		if (_destroyed) {
			return;
		}
		// Convert Transform::Position to sscreen position
		params.screenPosition = level->GetWorld().WorldToScreenPoint(transform->Position());
		// Convert Transform::Rotation to SFML orientation (+ is counter-clockwise)
		params.screenRotation = -transform->Rotation();
		// Render each component
		for (auto& component : components) {
			component->Render(params);
		}
		// Render collider if any
		if (collider != nullptr) {
			collider->Render(params);
		}
	}

	Level & Actor::GetActiveLevel() const {
		return *level;
	}
}
