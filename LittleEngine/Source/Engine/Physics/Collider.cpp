#include "le_stdafx.h"
#include <string>
#include "Collider.h"
#include "Entities/Actor.h"
#include "Engine/World.h"
#include "Levels/Level.h"
#include "CollisionManager.h"
#include "SFMLInterface/Rendering/RenderParams.h"
#include "SFMLInterface/Rendering/ShapeRenderer.h"
#include "Engine/Logger/Logger.h"
#include "Engine/Events/EventManager.h"

namespace LittleEngine {
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
	bool Collider::bShowingDebugShapes = false;

	Collider::~Collider() {
		debugOffToken = nullptr;
		debugOnToken = nullptr;
	}

	void Collider::OnHit(Collider & other) {
		static int DEBUG_skip = 0;
		if (++DEBUG_skip > 20) {
			Logger::Log(*this, "[" + GetActor().GetName() + "] colliding with [" + other.GetActor().GetName() + "] (throttled log)", Logger::Severity::Debug);
			DEBUG_skip = 0;
		}
	}

	Collider::Collider(Actor& actor, const std::string& name) : Component(actor, name) {
		this->world = &actor.GetActiveLevel().GetWorld();
		RegisterSubscribers();
	}

	Collider::Collider(Actor & owner, const Collider & prototype) : Component(owner, prototype) {
		this->world = &owner.GetActiveLevel().GetWorld();
		RegisterSubscribers();
	}

	bool Collider::IsShowingDebugShape() const {
		return bShowingDebugShapes;
	}

	void Collider::RegisterSubscribers() {
		debugOffToken = EventManager::Instance().Register(GameEvent::DEBUG_HIDE_COLLIDERS, [this]() { bShowingDebugShapes = false; DrawDebugShape(bShowingDebugShapes); });
		debugOnToken = EventManager::Instance().Register(GameEvent::DEBUG_SHOW_COLLIDERS, [this]() { bShowingDebugShapes = true; DrawDebugShape(bShowingDebugShapes); });
	}

	AABBCollider::AABBCollider(Actor& actor) : Collider(actor, "AABBCollider") {
		Vector2 size(bounds.lowerBound.x * 2, bounds.upperBound.y * 2);
		debugShape = std::make_unique<RectangleRenderer>(size, Colour::Transparent);
		debugShape->SetBorder(DEBUG_BORDER_WIDTH, Colour::Green);
		debugShape->layer = LayerID::Collider;
		debugShape->SetEnabled(IsShowingDebugShape());
	}

	AABBCollider::AABBCollider(Actor& owner, const AABBCollider & prototype) : Collider(owner, prototype), bounds(prototype.bounds) {
		ShapeRenderer* toCopy = prototype.debugShape.get();
		debugShape = std::make_unique<RectangleRenderer>(*(dynamic_cast<RectangleRenderer*>(toCopy)));
		debugShape->SetEnabled(IsShowingDebugShape());
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
		auto debugRect = dynamic_cast<RectangleRenderer*>(debugShape.get());
		debugRect->SetSize(size);
	}

	void AABBCollider::DrawDebugShape(bool bShow, const Fixed& thickness) {
		debugShape->SetEnabled(bShow);
		debugShape->SetBorder(thickness, Colour::Green);
		std::string prefix = bShow ? "Drawing " : "Hiding ";
		Logger::Log(*this, prefix + "debug collision rect on " + GetActor().GetName(), Logger::Severity::Debug);
	}

	void AABBCollider::Render(RenderParams params) {
		if (debugShape->IsEnabled()) {
			// Undo Actor's rotation (align with axes)
			params.screenRotation = 0;
			debugShape->Render(params);
		}
	}

	Component::Ptr AABBCollider::SClone(Actor& owner) const {
		return std::make_shared<AABBCollider>(owner, *this);
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
		debugShape = std::make_unique<CircleRenderer>(circle.radius, Colour::Transparent);
		debugShape->SetBorder(DEBUG_BORDER_WIDTH, Colour::Green);
		debugShape->layer = LayerID::Collider;
		debugShape->SetEnabled(IsShowingDebugShape());
	}

	CircleCollider::CircleCollider(Actor& owner, const CircleCollider & prototype) : Collider(owner, prototype.name), circle(prototype.circle) {
		ShapeRenderer* toCopy = prototype.debugShape.get();
		debugShape = std::make_unique<CircleRenderer>(*(dynamic_cast<CircleRenderer*>(toCopy)));
		debugShape->SetEnabled(IsShowingDebugShape());
	}

	bool CircleCollider::IsIntersecting(const Collider & rhs) const {
		return rhs.IsIntersectCircle(*this);
	}

	CircleData CircleCollider::GetWorldCircle() const {
		return CircleData(circle.radius, GetActor().GetTransform().Position());
	}

	void CircleCollider::SetCircle(Fixed radius) {
		circle.radius = radius;
		auto circleShape = dynamic_cast<CircleRenderer*>(debugShape.get());
		circleShape->SetRadius(radius);
	}

	void CircleCollider::DrawDebugShape(bool bShow, const Fixed& thickness) {
		debugShape->SetEnabled(bShow);
		debugShape->SetBorder(thickness, Colour::Green);
		std::string prefix = bShow ? "Drawing " : "Hiding ";
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

	Component::Ptr CircleCollider::SClone(Actor& owner) const {
		return std::make_shared<CircleCollider>(owner, *this);
	}
}
