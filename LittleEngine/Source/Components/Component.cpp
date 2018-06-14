#include "stdafx.h"
#include "Component.h"
#include "Engine/Logger/Logger.h"
#include "Entities/Actor.h"

namespace Game {
	Component::Component(Actor& actor, const std::string& name) : Object(name) {
		Logger::Log(*this, name + " created on " + actor.GetName());
		this->actor = &actor;
	}

	Component::~Component() {
		Logger::Log(*this, name + " destroyed on " + actor->GetName());
	}

	void Component::Tick(Fixed deltaSeconds) {
	}

	void Component::FixedTick() {
	}

	void Component::Render(RenderParams& params) {
	}

	Actor& Component::GetActor() const {
		return *actor;
	}
}
