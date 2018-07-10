#include "stdafx.h"
#include <string>
#include "Collider.h"
#include "Entities/Actor.h"
#include "Levels/Level.h"
#include "CollisionManager.h"
#include "Engine/Logger/Logger.h"

namespace Game {
	Collider::Collider(Actor& actor, const std::string& name) : Component(actor, name) {
	}

	Collider::~Collider() = default;

	CollisionManager & Collider::GetCollisionManager() {
		return GetActor().GetActiveLevel().GetCollisionManager();
	}

	BoxCollider::BoxCollider(Actor& actor) : Collider(actor, "BoxCollider") {
	}
}
