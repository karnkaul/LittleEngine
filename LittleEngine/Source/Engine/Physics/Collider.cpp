#include "stdafx.h"
#include <string>
#include "Collider.h"
#include "Entities/Actor.h"
#include "Engine/World.h"
#include "Levels/Level.h"
#include "CollisionManager.h"
#include "SFMLInterface/Rendering/RenderParams.h"
#include "SFMLInterface/Rendering/ShapeRenderer.h"
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

	Fixed Collider::DEBUG_BORDER_WIDTH = 1;

	Collider::Collider(Actor& actor, std::string name) : Component(actor, name), world(actor.GetActiveLevel().GetWorld()) {
	}

	AABBCollider::AABBCollider(Actor& actor) : Collider(actor, "AABBCollider") {
	}

	bool AABBCollider::IsIntersecting(const Collider & rhs) const {
		return rhs.IsIntersectAABB(*this);
	}

	AABBData AABBCollider::GetWorldAABB() const {
		Vector2 displacement = GetActor().GetTransform().Position();
		return AABBData(bounds.lowerBound + displacement, bounds.upperBound + displacement);
	}

	void AABBCollider::SetBounds(AABBData bounds) {
		this->bounds = bounds;
	}

	void AABBCollider::DrawDebugShape(bool show, Fixed thickness) {
		if (show) {
			Vector2 size(bounds.lowerBound.x * 2, bounds.upperBound.y * 2);
			debugRect = std::make_unique<RectangleRenderer>(size, Colour::Transparent);
			debugRect->SetBorder(thickness, Colour::Green);
			debugRect->layer = LayerID::Collider;
			Logger::Log(*this, "Drawing debug collision rect", Logger::Severity::Debug);
		}
		else {
			debugRect = nullptr;
			Logger::Log(*this, "Hiding debug collision rect", Logger::Severity::Debug);
		}
	}

	void AABBCollider::Render(RenderParams & params) {
		if (debugRect != nullptr) {
			AABBData worldRect = GetWorldAABB();
			Fixed w = worldRect.upperBound.x - worldRect.lowerBound.x;
			Fixed h = worldRect.upperBound.y - worldRect.lowerBound.y;
			Vector2 delta(w * Fixed::Half, h * Fixed::Half);
			params.screenPosition = world.WorldToScreenPoint(worldRect.lowerBound + delta);
			debugRect->Render(params);
		}
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
		return CircleData(circle.radius, GetActor().GetTransform().Position());
	}

	void CircleCollider::SetCircle(Fixed radius) {
		circle.radius = radius;
	}

	void CircleCollider::DrawDebugShape(bool show, Fixed thickness) {
		if (show) {
			debugCircle = std::make_unique<CircleRenderer>(circle.radius, Colour::Transparent);
			debugCircle->SetBorder(thickness, Colour::Green);
			debugCircle->layer = LayerID::Collider;
			Logger::Log(*this, "Drawing debug collision circle", Logger::Severity::Debug);
		}
		else {
			debugCircle = nullptr;
			Logger::Log(*this, "Hiding debug collision circle", Logger::Severity::Debug);
		}
	}

	bool CircleCollider::IsIntersectAABB(const AABBCollider & rhs) const {
		return rhs.IsIntersectCircle(*this);
	}

	bool CircleCollider::IsIntersectCircle(const CircleCollider & rhs) const {
		CircleData lhsCircle = GetWorldCircle();
		CircleData rhsCircle = rhs.GetWorldCircle();
		return lhsCircle.IsIntersecting(rhsCircle);
	}

	void CircleCollider::Render(RenderParams& params) {
		if (debugCircle != nullptr) {
			CircleData c = GetWorldCircle();
			params.screenPosition = world.WorldToScreenPoint(c.centre);
			debugCircle->Render(params);
		}
	}
}
