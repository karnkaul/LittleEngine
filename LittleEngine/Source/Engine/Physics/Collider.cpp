#include "stdafx.h"
#include <string>
#include "Collider.h"
#include "Entities/Actor.h"
#include "Levels/Level.h"
#include "CollisionManager.h"
#include "Engine/Logger/Logger.h"

namespace Game {
	Collider::Collider(Actor& actor) : Component(actor, "Collider") {
	}

	CollisionManager & Collider::GetCollisionManager() {
		return GetActor().GetActiveLevel().GetCollisionManager();
	}

	AABB Collider::GetWorldAABB() const {
		return bounds + GetActor().GetTransform()->localPosition;
	}

	void Collider::SetBounds(AABB bounds) {
		this->bounds = bounds;
	}
}
