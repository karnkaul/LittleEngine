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
	Actor::Actor(Level& level, std::string name) : Object(name) {
		this->level = &level;
		transform = std::make_shared<Transform>();
		Logger::Log(*this, "Created new actor \"" + name + "\" at " + transform->Position().ToString());
	}

	Actor::~Actor() {
		components.clear();
		Logger::Log(*this, "Destroyed actor \"" + name + "\"");
	}

	std::shared_ptr<Transform> Actor::GetTransform() const {
		return transform;
	}

	std::string Actor::ToString() const {
		return name + " : " + transform->ToString();
	}

	void Actor::FixedTick() {
		for (const auto& component : components) {
			if (component != nullptr && component->enabled) {
				component->FixedTick();
			}
		}
	}

	void Actor::Tick(Fixed deltaTime) {
		for (const auto& component : components) {
			if (component->enabled) {
				component->Tick(deltaTime);
			}
		}
	}

	void Actor::Render(RenderParams& params) {
		for (auto& component : components) {
			component->Render(params);
		}
	}

	Level & Actor::GetActiveLevel() const {
		return *level;
	}

	std::shared_ptr<Actor> Actor::This() {
		return this->shared_from_this();
	}
}
