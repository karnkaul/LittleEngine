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

namespace Game {
	Actor::Actor(Level& level, std::string name) : Object(name), level(level) {
		transform = Transform::Create();
		Logger::Log(*this, "Created new actor \"" + name + "\" at " + transform->Position().ToString());
	}

	Actor::~Actor() {
		if (collider != nullptr && GetActiveLevel().GetCollisionManager().Unregister(collider)) {
			collider = nullptr;
		}
		components.clear();
		Logger::Log(*this, "Destroyed actor \"" + name + "\"");
	}

	Transform::Ptr Actor::GetTransform() const {
		return transform;
	}

	std::string Actor::ToString() const {
		return name + " : " + transform->ToString();
	}

	void Actor::Destruct() {
		_destroyed = true;
	}

	void Actor::FixedTick() {
		for (const auto& component : components) {
			if (component != nullptr && component->enabled) {
				component->FixedTick();
			}
		}
		if (collider != nullptr) {
			collider->FixedTick();
		}
	}

	void Actor::Tick(Fixed deltaTime) {
		for (const auto& component : components) {
			if (component->enabled) {
				component->Tick(deltaTime);
			}
		}
		if (collider != nullptr) {
			collider->Tick(deltaTime);
		}
	}

	void Actor::Render(RenderParams& params) {
		for (auto& component : components) {
			component->Render(params);
		}
		if (collider != nullptr) {
			collider->Render(params);
		}
	}

	Level & Actor::GetActiveLevel() const {
		return level;
	}
}
