#include "le_stdafx.h"
#include "Component.h"
#include "Engine/Logger/Logger.h"
#include "Entities/Actor.h"
#include "SFMLInterface/Rendering/RenderParams.h"

namespace LittleEngine {
	Component::Component(Actor& actor, const std::string& name) : Object(name), m_pActor(&actor) {
		Logger::Log(*this, ToString() + " created");
	}

	Component::Component(Actor& owner, const Component& prototype) : Object(prototype.m_name + "_clone"), m_pActor(&owner) {
		Logger::Log(*this, ToString() + " cloned");
	}

	Component::~Component() {
		Logger::Log(*this, ToString() + " destroyed");
	}

	void Component::Tick(const Fixed&) {
	}

	void Component::FixedTick() {
	}

	void Component::Render(const RenderParams&) {
	}

	Actor& Component::GetActor() const {
		return *m_pActor;
	}

	Component::Ptr Component::UClone(Actor&) const {
		return nullptr;
	}

	std::string Component::ToString() const {
		return "[" + m_pActor->GetName() + ":" + GetName() + "]";
	}
}
