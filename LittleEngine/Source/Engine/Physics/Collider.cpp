#include "stdafx.h"
#include <string>
#include "Collider.h"
#include "Entities/Actor.h"
#include "Levels/Level.h"
#include "CollisionManager.h"
#include "Engine/Logger/Logger.h"

namespace Game {
	struct CircleLocus {
		CircleData topLeft;
		CircleData topRight;
		CircleData bottomLeft;
		CircleData bottomRight;
		AABBData horizontal;
		AABBData vertical;
		CircleLocus() = default;
		CircleLocus(CircleLocus&&) = default;
		CircleLocus& operator=(CircleLocus&&) = default;

		bool IsPointIn(Vector2 point) {
			return topLeft.IsPointInCircle(point) ||
				topRight.IsPointInCircle(point) ||
				bottomLeft.IsPointInCircle(point) ||
				bottomRight.IsPointInCircle(point) ||
				horizontal.IsPointInRect(point) ||
				vertical.IsPointInRect(point);
		}
	};
	
	CircleLocus Intersect(AABBData bounds, CircleData circle) {
		CircleLocus ret;
		ret.topLeft.centre = Vector2(bounds.lowerBound.x, bounds.upperBound.y);
		ret.topRight.centre = Vector2(bounds.upperBound.x, bounds.upperBound.y);
		ret.bottomLeft.centre = Vector2(bounds.lowerBound.x, bounds.lowerBound.y);
		ret.bottomRight.centre = Vector2(bounds.upperBound.x, bounds.lowerBound.y);
		ret.topLeft.radius = ret.topRight.radius = ret.bottomLeft.radius 
			= ret.bottomRight.radius = circle.radius;
		ret.horizontal = AABBData(Vector2(bounds.lowerBound.x - circle.radius, bounds.lowerBound.y),
								  Vector2(bounds.upperBound.x + circle.radius, bounds.upperBound.y));
		ret.vertical = AABBData(Vector2(bounds.lowerBound.x, bounds.lowerBound.y - circle.radius), 
								Vector2(bounds.upperBound.x, bounds.upperBound.y + circle.radius));
		return ret;
	}

	Collider::Collider(Actor& actor, std::string name) : Component(actor, name) {
	}

	AABBCollider::AABBCollider(Actor& actor) : Collider(actor, "AABBCollider") {
	}

	bool AABBCollider::IsIntersecting(const Collider & rhs) const {
		return rhs.IsIntersectAABB(*this);
	}

	AABBData AABBCollider::GetWorldAABB() const {
		Vector2 displacement = GetActor().GetTransform()->Position();
		return AABBData(bounds.lowerBound + displacement, bounds.upperBound + displacement);
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
		CircleData circle = rhs.GetWorldCircle();
		CircleLocus locus = Intersect(GetWorldAABB(), circle);
		return locus.IsPointIn(circle.centre);
	}

	CircleCollider::CircleCollider(Actor& actor) : Collider(actor, "CircleCollider") {
	}

	bool CircleCollider::IsIntersecting(const Collider & rhs) const {
		return rhs.IsIntersectCircle(*this);
	}

	CircleData CircleCollider::GetWorldCircle() const {
		return CircleData(circle.radius, GetActor().GetTransform()->Position());
	}

	void CircleCollider::SetCircle(Fixed radius) {
		circle.radius = radius;
	}

	bool CircleCollider::IsIntersectAABB(const AABBCollider & rhs) const {
		return rhs.IsIntersectCircle(*this);
	}

	bool CircleCollider::IsIntersectCircle(const CircleCollider & rhs) const {
		CircleData lhsCircle = GetWorldCircle();
		CircleData rhsCircle = rhs.GetWorldCircle();
		return lhsCircle.IsIntersecting(rhsCircle);
	}
}
