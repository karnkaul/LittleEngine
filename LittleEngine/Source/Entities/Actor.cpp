#include "le_stdafx.h"
#include <iostream>

#include "Actor.h"
#include "Engine/Logger/Logger.h"
#include "Engine/World.h"
#include "Levels/Level.h"
#include "Levels/LevelManager.h"
#include "SFMLInterface/Assets.h"
#include "SFMLInterface/Rendering/RenderParams.h"
#include "SFMLInterface/Rendering/Renderer.h"
#include "SFMLInterface/Rendering/SpriteRenderer.h"
#include "Components/Component.h"
#include "Utils.h"

namespace LittleEngine {
	Actor::Actor(Level& level, const std::string& name, const Vector2& position, const Fixed& rotation) : Object(name), level(&level) {
		transform.localPosition = position;
		transform.localRotation = rotation;
		Logger::Log(*this, GetNameInBrackets() + " (Actor) spawned at " + transform.Position().ToString());
	}

	Actor::Actor(Level& owner, const Actor& prototype) : Object(prototype.name + "_clone"), level(&owner), transform(prototype.transform) {
		transform.localPosition = prototype.transform.localPosition;
		transform.localRotation = prototype.transform.localRotation;
		for (const auto& toImport : prototype.components) {
			components.emplace_back(toImport->UClone(*this));
		}
		if (prototype.collider) {
			collider = GetActiveLevel().GetCollisionManager().CreateCollider(*this, *prototype.collider);
		}
		Logger::Log(*this, GetNameInBrackets() + " (Actor) cloned at " + transform.Position().ToString());
	}

	Actor::~Actor() {
		if (collider && GetActiveLevel().GetCollisionManager().Unregister(*collider)) {
			collider = nullptr;
		}
		tokenHandler.Clear();
		components.clear();
		Logger::Log(*this, GetNameInBrackets() + " (Actor) Destroyed");
	}

	void Actor::SetNormalisedPosition(Vector2 localNPosition) {
		transform.localPosition = GetActiveLevel().GetWorld().NormalisedToWorldPoint(localNPosition);
	}

	std::string Actor::ToString() const {
		return name + " : " + transform.ToString();
	}

	void Actor::Destruct() {
		// Allow ActiveLevel to collect this Actor
		_bDestroyed = true;
	}

	void Actor::ToggleActive(bool enable) {
		this->_bEnabled = enable;
	}

	void Actor::FixedTick() {
		// Don't do anything if about to be destroyed
		if (_bDestroyed) {
			return;
		}
		// FixedTick each component
		for (const auto& component : components) {
			if (component && component->bEnabled) {
				component->FixedTick();
			}
		}
		// FixedTick collider if any
		if (collider) {
			collider->FixedTick();
		}
	}

	void Actor::Tick(Fixed deltaTime) {
		// Don't do anything if about to be destroyed
		if (_bDestroyed) {
			return;
		}
		// Tick each enabled component
		for (const auto& component : components) {
			if (component && component->bEnabled) {
				component->Tick(deltaTime);
			}
		}
		// Tick collider if any
		if (collider) {
			collider->Tick(deltaTime);
		}
	}

	void Actor::Render(RenderParams& params) {
		// Don't add to render buffer if about to be destroyed
		if (_bDestroyed) {
			return;
		}
		// Convert Transform::Position to Screen position
		params.screenPosition = level->GetWorld().WorldToScreenPoint(transform.Position());
		// Convert Transform::Rotation to SFML orientation (+ is counter-clockwise)
		params.screenRotation = -Fixed(transform.Rotation());
		// Render each component
		for (auto& component : components) {
			if (component && component->bEnabled) {
				component->Render(params);
			}
		}
		// Render collider if any
		if (collider) {
			collider->Render(params);
		}
	}

	void Actor::RegisterScopedInput(const GameInput & gameInput, OnInput::Callback callback, const OnKey & type, bool consume) {
		OnInput::Token token = level->GetInputHandler().Register(gameInput, callback, type, consume);
		tokenHandler.AddToken(std::move(token));
	}

	Level & Actor::GetActiveLevel() const {
		return *level;
	}

	int Actor::GetActorID() const {
		return actorID;
	}
}
