#include "stdafx.h"
#include <string>
#include "Collider.h"
#include "Entities/Actor.h"
#include "Levels/Level.h"
#include "CollisionManager.h"
#include "Engine/Logger/Logger.h"

namespace Game {
	Collider::Collider(Actor& actor, std::string name) : Component(actor, name) {
	}

	AABBCollider::AABBCollider(Actor& actor) : Collider(actor, "AABBCollider") {
	}

	bool AABBCollider::IsIntersecting(const Collider & rhs) const {
		return rhs.IsIntersectAABB(*this);
	}

	AABBData AABBCollider::GetWorldAABB() const {
		return bounds + GetActor().GetTransform()->Position();
	}

	void AABBCollider::SetBounds(AABBData bounds) {
		this->bounds = bounds;
	}

	bool AABBCollider::IsIntersectAABB(const AABBCollider & rhs) const {
		AABBData lhsBounds = GetWorldAABB();
		AABBData rhsBounds = rhs.GetWorldAABB();
		return lhsBounds.Intersecting(rhsBounds);
	}

	bool AABBCollider::IsIntersectCircle(const CircleCollider & rhs) const {
		return false;
	}

	CircleCollider::CircleCollider(Actor& actor) : Collider(actor, "CircleCollider") {
	}

	bool CircleCollider::IsIntersecting(const Collider & rhs) const {
		return rhs.IsIntersectCircle(*this);
	}

	Fixed CircleCollider::GetRadius() const {
		return radius;
	}

	void CircleCollider::SetRadius(Fixed radius) {
		this->radius = radius;
	}

	bool CircleCollider::IsIntersectAABB(const AABBCollider & rhs) const {
		return rhs.IsIntersectCircle(*this);
	}

	bool CircleCollider::IsIntersectCircle(const CircleCollider & rhs) const {
		Vector2 lhsPos = GetActor().GetTransform()->Position();
		Vector2 rhsPos = rhs.GetActor().GetTransform()->Position();
		Fixed centreDist = (rhsPos - lhsPos).Magnitude();
		Fixed radiiDist = rhs.radius + radius;
		return radiiDist >= centreDist;
	}
}
