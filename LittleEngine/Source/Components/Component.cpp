#include "le_stdafx.h"
#include "Component.h"
#include "Engine/Logger/Logger.h"
#include "Entities/Actor.h"
#include "SFMLInterface/Rendering/RenderParams.h"

namespace LittleEngine {
	Component::Component(Actor& actor, const std::string& name) : Object(name), actor(&actor) {
		Logger::Log(*this, ToString() + " created");
	}

	Component::Component(Actor& owner, const Component& prototype) : Object(prototype.name + "_clone"), actor(&owner) {
		Logger::Log(*this, ToString() + " cloned");
	}

	Component::~Component() {
		Logger::Log(*this, ToString() + " destroyed");
	}

	void Component::Tick(Fixed deltaSeconds) {
	}

	void Component::FixedTick() {
	}

	void Component::Render(RenderParams params) {
	}

	Actor& Component::GetActor() const {
		return *actor;
	}

	Component::Ptr Component::UClone(Actor& owner) const {
		return nullptr;
	}

	std::string Component::ToString() const {
		return "[" + actor->GetName() + ":" + GetName() + "]";
	}
}
