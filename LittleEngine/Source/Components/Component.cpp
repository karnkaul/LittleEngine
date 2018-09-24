#include "stdafx.h"
#include "Component.h"
#include "Engine/Logger/Logger.h"
#include "Entities/Actor.h"
#include "SFMLInterface/Rendering/RenderParams.h"

namespace LittleEngine {
	Component::Component(Actor& actor, const std::string& name) : Object(name), actor(&actor) {
		Logger::Log(*this, ToString() + " created");
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

	std::string Component::ToString() const {
		return "[" + actor->GetName() + ":" + GetName() + "]";
	}
}
