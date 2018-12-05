#include "le_stdafx.h"
#include <string>
#include "Collider.h"
#include "Entities/Actor.h"
#include "Engine/World.h"
#include "Levels/Level.h"
#include "CollisionManager.h"
#include "SFMLInterface/Rendering/RenderParams.h"
#include "SFMLInterface/Rendering/ShapeRenderable.h"
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
	bool Collider::m_bShowingDebugShapes = false;

	Collider::~Collider() {
		m_debugOffToken = nullptr;
		m_debugOnToken = nullptr;
	}

	void Collider::OnHit(Collider & other) {
		static int DEBUG_skip = 0;
		if (++DEBUG_skip > 20) {
			Logger::Log(*this, "[" + GetActor().GetName() + "] colliding with [" + other.GetActor().GetName() + "] (throttled log)", Logger::Severity::Debug);
			DEBUG_skip = 0;
		}
	}

	Collider::Collider(Actor& actor, const std::string& name) : Component(actor, name) {
		this->m_pWorld = &actor.GetActiveLevel().GetWorld();
		RegisterSubscribers();
	}

	Collider::Collider(Actor & owner, const Collider & prototype) : Component(owner, prototype) {
		this->m_pWorld = &owner.GetActiveLevel().GetWorld();
		RegisterSubscribers();
	}

	bool Collider::IsShowingDebugShape() const {
		return m_bShowingDebugShapes;
	}

	void Collider::RegisterSubscribers() {
		m_debugOffToken = EventManager::Instance().Register(GameEvent::DEBUG_HIDE_COLLIDERS, [this]() { m_bShowingDebugShapes = false; DrawDebugShape(m_bShowingDebugShapes); });
		m_debugOnToken = EventManager::Instance().Register(GameEvent::DEBUG_SHOW_COLLIDERS, [this]() { m_bShowingDebugShapes = true; DrawDebugShape(m_bShowingDebugShapes); });
	}

	AABBCollider::AABBCollider(Actor& actor) : Collider(actor, "AABBCollider") {
		Vector2 size(m_bounds.lowerBound.x * 2, m_bounds.upperBound.y * 2);
		m_debugShape = std::make_unique<RectangleRenderable>(size, Colour::Transparent);
		m_debugShape->SetBorder(DEBUG_BORDER_WIDTH, Colour::Green);
		m_debugShape->m_layer = LayerID::Collider;
		m_debugShape->SetEnabled(IsShowingDebugShape());
	}

	AABBCollider::AABBCollider(Actor& owner, const AABBCollider & prototype) : Collider(owner, prototype), m_bounds(prototype.m_bounds) {
		ShapeRenderable* toCopy = prototype.m_debugShape.get();
		m_debugShape = std::make_unique<RectangleRenderable>(*(dynamic_cast<RectangleRenderable*>(toCopy)));
		m_debugShape->SetEnabled(IsShowingDebugShape());
	}

	bool AABBCollider::IsIntersecting(const Collider & rhs) const {
		return rhs.IsIntersectAABB(*this);
	}

	AABBData AABBCollider::GetWorldAABB() const {
		Vector2 displacement = GetActor().GetTransform().Position();
		return AABBData(m_bounds.lowerBound + displacement, m_bounds.upperBound + displacement);
	}

	void AABBCollider::SetBounds(AABBData bounds) {
		this->m_bounds = bounds;
		Vector2 size(bounds.lowerBound.x * 2, bounds.upperBound.y * 2);
		auto debugRect = dynamic_cast<RectangleRenderable*>(m_debugShape.get());
		debugRect->SetSize(size);
	}

	void AABBCollider::DrawDebugShape(bool bShow, const Fixed& thickness) {
		m_debugShape->SetEnabled(bShow);
		m_debugShape->SetBorder(thickness, Colour::Green);
		std::string prefix = bShow ? "Drawing " : "Hiding ";
		Logger::Log(*this, prefix + "debug collision rect on " + GetActor().GetName(), Logger::Severity::Debug);
	}

	void AABBCollider::Render(RenderParams& params) {
		if (m_debugShape->IsEnabled()) {
			// Reset rotation (align with axes) and scale
			params.screenRotation = Fixed::Zero;
			params.screenScale = Vector2::One;
			m_debugShape->Render(params);
		}
	}

	Component::Ptr AABBCollider::UClone(Actor& owner) const {
		return std::make_unique<AABBCollider>(owner, *this);
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
		m_debugShape = std::make_unique<CircleRenderable>(m_circle.radius, Colour::Transparent);
		m_debugShape->SetBorder(DEBUG_BORDER_WIDTH, Colour::Green);
		m_debugShape->m_layer = LayerID::Collider;
		m_debugShape->SetEnabled(IsShowingDebugShape());
	}

	CircleCollider::CircleCollider(Actor& owner, const CircleCollider & prototype) : Collider(owner, prototype.m_name), m_circle(prototype.m_circle) {
		ShapeRenderable* toCopy = prototype.m_debugShape.get();
		m_debugShape = std::make_unique<CircleRenderable>(*(dynamic_cast<CircleRenderable*>(toCopy)));
		m_debugShape->SetEnabled(IsShowingDebugShape());
	}

	bool CircleCollider::IsIntersecting(const Collider & rhs) const {
		return rhs.IsIntersectCircle(*this);
	}

	CircleData CircleCollider::GetWorldCircle() const {
		return CircleData(m_circle.radius, GetActor().GetTransform().Position());
	}

	void CircleCollider::SetCircle(Fixed radius) {
		m_circle.radius = radius;
		auto circleShape = dynamic_cast<CircleRenderable*>(m_debugShape.get());
		circleShape->SetRadius(radius);
	}

	void CircleCollider::DrawDebugShape(bool bShow, const Fixed& thickness) {
		m_debugShape->SetEnabled(bShow);
		m_debugShape->SetBorder(thickness, Colour::Green);
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

	void CircleCollider::Render(RenderParams& params) {
		if (m_debugShape->IsEnabled()) {
			// Undo Actor's rotation (be pedantic)
			params.screenRotation = 0;
			m_debugShape->Render(params);
		}
	}

	Component::Ptr CircleCollider::UClone(Actor& owner) const {
		return std::make_unique<CircleCollider>(owner, *this);
	}
}
