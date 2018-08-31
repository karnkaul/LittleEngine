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
	// \brief Wrapper to calculate locus of intersection of Circle with an AABB:
	// Calculated via tracing centre of circle along radial boundary of AABB.
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

		bool IsPointIn(const Vector2& point) {
			return topLeft.IsPointInCircle(point) ||
				topRight.IsPointInCircle(point) ||
				bottomLeft.IsPointInCircle(point) ||
				bottomRight.IsPointInCircle(point) ||
				horizontal.IsPointInRect(point) ||
				vertical.IsPointInRect(point);
		}
	};
	
	CircleLocus Intersect(const AABBData& bounds, const CircleData& circle) {
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

	Collider::Collider(Actor& actor, const std::string& name) : Component(actor, name) {
		this->world = &actor.GetActiveLevel().GetWorld();
	}

	AABBCollider::AABBCollider(Actor& actor) : Collider(actor, "AABBCollider") {
		Vector2 size(bounds.lowerBound.x * 2, bounds.upperBound.y * 2);
		debugShape = std::make_shared<RectangleRenderer>(size, Colour::Transparent);
		debugShape->SetBorder(DEBUG_BORDER_WIDTH, Colour::Green);
		debugShape->layer = LayerID::Collider;
		debugShape->SetEnabled(false);
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
		Vector2 size(bounds.lowerBound.x * 2, bounds.upperBound.y * 2);
		auto debugRect = std::dynamic_pointer_cast<RectangleRenderer>(debugShape);
		debugRect->SetSize(size);
	}

	void AABBCollider::DrawDebugShape(bool show, const Fixed& thickness) {
		debugShape->SetEnabled(show);
		debugShape->SetBorder(thickness, Colour::Green);
		std::string prefix = show ? "Drawing " : "Hiding ";
		Logger::Log(*this, prefix + "debug collision rect on " + GetActor().GetName(), Logger::Severity::Debug);
	}

	void AABBCollider::Render(RenderParams params) {
		if (debugShape->IsEnabled()) {
			// Undo Actor's rotation (align with axes)
			params.screenRotation = 0;
			debugShape->Render(params);
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
		debugShape = std::make_shared<CircleRenderer>(circle.radius, Colour::Transparent);
		debugShape->SetBorder(DEBUG_BORDER_WIDTH, Colour::Green);
		debugShape->layer = LayerID::Collider;
		debugShape->SetEnabled(false);
	}

	bool CircleCollider::IsIntersecting(const Collider & rhs) const {
		return rhs.IsIntersectCircle(*this);
	}

	CircleData CircleCollider::GetWorldCircle() const {
		return CircleData(circle.radius, GetActor().GetTransform().Position());
	}

	void CircleCollider::SetCircle(Fixed radius) {
		circle.radius = radius;
		auto circleShape = std::dynamic_pointer_cast<CircleRenderer>(debugShape);
		circleShape->SetRadius(radius);
	}

	void CircleCollider::DrawDebugShape(bool show, const Fixed& thickness) {
		debugShape->SetEnabled(show);
		debugShape->SetBorder(thickness, Colour::Green);
		std::string prefix = show ? "Drawing " : "Hiding ";
		Logger::Log(*this, prefix + "debug collision circle on " + GetActor().GetName(), Logger::Severity::Debug);
	}

	bool CircleCollider::IsIntersectAABB(const AABBCollider & rhs) const {
		return rhs.IsIntersectCircle(*this);
	}

	bool CircleCollider::IsIntersectCircle(const CircleCollider & rhs) const {
		CircleData lhsCircle = GetWorldCircle();
		CircleData rhsCircle = rhs.GetWorldCircle();
		return lhsCircle.IsIntersecting(rhsCircle);
	}

	void CircleCollider::Render(RenderParams params) {
		if (debugShape->IsEnabled()) {
			// Undo Actor's rotation (be pedantic)
			params.screenRotation = 0;
			debugShape->Render(params);
		}
	}
}
