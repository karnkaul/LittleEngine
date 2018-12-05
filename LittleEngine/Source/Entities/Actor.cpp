#include "le_stdafx.h"
#include <iostream>

#include "Actor.h"
#include "Engine/Logger/Logger.h"
#include "Engine/World.h"
#include "Levels/Level.h"
#include "Levels/LevelManager.h"
#include "SFMLInterface/Assets.h"
#include "SFMLInterface/Rendering/RenderParams.h"
#include "SFMLInterface/Rendering/Renderable.h"
#include "SFMLInterface/Rendering/SpriteRenderable.h"
#include "Components/Component.h"
#include "Utils.h"

namespace LittleEngine {
	const std::string Actor::UNNAMED_ACTOR = "Unnamed_Actor";

	Actor::Actor() : Object(UNNAMED_ACTOR) {
		Logger::Log(*this, "Actor default constructed", Logger::Severity::Debug);
	}

	void Actor::InitActor(Level& level, int actorID, const std::string& name, const Vector2& position, const Fixed& rotation) {
		GeneralInit(level, actorID, name);
		m_transform.localPosition = position;
		m_transform.localOrientation = rotation;
		Logger::Log(*this, GetNameInBrackets() + " (Actor) spawned at " + m_transform.Position().ToString());
	}

	void Actor::InitActor(Level& owner, int actorID, const Actor& prototype) {
		GeneralInit(owner, actorID, prototype.m_name + "_clone");
		m_transform = prototype.m_transform;
		for (const auto& toImport : prototype.m_components) {
			m_components.emplace_back(toImport->UClone(*this));
		}
		if (prototype.m_sCollider) {
			m_sCollider = GetActiveLevel().GetCollisionManager().CreateCollider(*this, *prototype.m_sCollider);
		}
		Logger::Log(*this, GetNameInBrackets() + " (Actor) cloned at " + m_transform.Position().ToString());
	}

	void Actor::GeneralInit(Level& level, int actorID, const std::string& name) {
		this->m_pLevel = &level;
		this->m_name = name;
		this->m_actorID = actorID;
	}

	Actor::~Actor() {
		if (m_sCollider && GetActiveLevel().GetCollisionManager().Unregister(*m_sCollider)) {
			m_sCollider = nullptr;
		}
		m_tokenHandler.Clear();
		m_components.clear();
		Logger::Log(*this, GetNameInBrackets() + " (Actor) Destroyed");
	}

	void Actor::SetNormalisedPosition(Vector2 localNPosition) {
		m_transform.localPosition = GetActiveLevel().GetWorld().NormalisedToWorldPoint(localNPosition);
	}

	std::string Actor::ToString() const {
		return m_name + " : " + m_transform.ToString();
	}

	void Actor::Destruct() {
		// Allow ActiveLevel to collect this Actor
		m_bDestroyed = true;
	}

	void Actor::ToggleActive(bool enable) {
		this->m_bEnabled = enable;
	}

	void Actor::FixedTick() {
		if (m_bDestroyed) return;
		
		// FixedTick Components and Collider
		for (const auto& component : m_components) {
			if (component && component->m_bEnabled) {
				component->FixedTick();
			}
		}
		if (m_sCollider) m_sCollider->FixedTick();
	}

	void Actor::Tick(const Fixed& deltaTime) {
		if (m_bDestroyed) return;

		// Tick Components and Collider
		for (const auto& component : m_components) {
			if (component && component->m_bEnabled) {
				component->Tick(deltaTime);
			}
		}
		if (m_sCollider) m_sCollider->Tick(deltaTime);
	}

	void Actor::Render(RenderParams& params) {
		if (m_bDestroyed) return;

		params.Reset();
		params.screenPosition = m_pLevel->GetWorld().WorldToScreenPoint(m_transform.Position());
		params.screenRotation = m_pLevel->GetWorld().WorldToScreenRotation(m_transform.Orientation());
		params.screenScale = m_transform.Scale();
		
		// Render Components and Collider
		for (auto& component : m_components) {
			if (component && component->m_bEnabled) {
				component->Render(params);
			}
		}
		if (m_sCollider) m_sCollider->Render(params);
	}

	void Actor::RegisterScopedInput(const GameInput & gameInput, OnInput::Callback callback, const OnKey & type, bool consume) {
		OnInput::Token token = m_pLevel->GetInputHandler().Register(gameInput, callback, type, consume);
		m_tokenHandler.AddToken(std::move(token));
	}

	Level & Actor::GetActiveLevel() const {
		return *m_pLevel;
	}

	int Actor::GetActorID() const {
		return m_actorID;
	}
}
