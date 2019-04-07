#include "stdafx.h"
#include "Logger.h"
#include "Collider.h"

namespace LittleEngine
{
namespace
{
struct CircleLocus
{
	CircleData topLeft;
	CircleData topRight;
	CircleData bottomLeft;
	CircleData bottomRight;
	AABBData horizontal;
	AABBData vertical;
	CircleLocus() = default;
	CircleLocus(CircleLocus&&) = default;
	CircleLocus& operator=(CircleLocus&&) = default;

	bool IsPointIn(Vector2 point)
	{
		return topLeft.IsPointInCircle(point) || topRight.IsPointInCircle(point) ||
			   bottomLeft.IsPointInCircle(point) || bottomRight.IsPointInCircle(point) ||
			   horizontal.IsPointInRect(point) || vertical.IsPointInRect(point);
	}
};

CircleLocus Intersect(const AABBData& bounds, const CircleData& circle)
{
	CircleLocus ret;
	ret.topLeft.centre = Vector2(bounds.lowerBound.x, bounds.upperBound.y);
	ret.topRight.centre = Vector2(bounds.upperBound.x, bounds.upperBound.y);
	ret.bottomLeft.centre = Vector2(bounds.lowerBound.x, bounds.lowerBound.y);
	ret.bottomRight.centre = Vector2(bounds.upperBound.x, bounds.lowerBound.y);
	ret.topLeft.radius = ret.topRight.radius = ret.bottomLeft.radius = ret.bottomRight.radius =
		circle.radius;
	ret.horizontal = AABBData(Vector2(bounds.lowerBound.x - circle.radius, bounds.lowerBound.y),
							  Vector2(bounds.upperBound.x + circle.radius, bounds.upperBound.y));
	ret.vertical = AABBData(Vector2(bounds.lowerBound.x, bounds.lowerBound.y - circle.radius),
							Vector2(bounds.upperBound.x, bounds.upperBound.y + circle.radius));
	return ret;
}
} // namespace

#if DEBUGGING
u32 Collider::s_debugShapeWidth = 1;
bool Collider::s_bShowDebugShape = false;
#endif

Collider::~Collider() = default;

#if DEBUG_LOGGING
void Collider::OnHit(const Collider& other)
{
	static int DEBUG_skip = 0;
	if (++DEBUG_skip > 20)
	{
		LOG_D("[%s] colliding with [%s] (throttled log)", m_name.c_str(), other.m_name.c_str());
		DEBUG_skip = 0;
	}
}
#else
void Collider::OnHit(const Collider&)
{
}
#endif

CircleCollider::CircleCollider(const String& ownerName)
{
	String prefix = ownerName.empty() ? "" : ownerName + "-";
	m_name = prefix + "CircleCollider";
}

void CircleCollider::SetCircle(Fixed radius)
{
	m_circle.radius = radius;
}

CircleData CircleCollider::GetWorldCircle() const
{
	return CircleData(m_circle.radius, m_position);
}

bool CircleCollider::IsIntersecting(const Collider& other) const
{
	return other.IsIntersectCircle(*this);
}

bool CircleCollider::IsIntersectAABB(const AABBCollider& other) const
{
	CircleData self = GetWorldCircle();
	CircleLocus locus = Intersect(other.GetWorldAABB(), self);
	return locus.IsPointIn(self.centre);
}

bool CircleCollider::IsIntersectCircle(const CircleCollider& other) const
{
	CircleData self = GetWorldCircle();
	CircleData rhs = other.GetWorldCircle();
	return self.IsIntersecting(rhs);
}

AABBCollider::AABBCollider(const String& ownerName)
{
	String prefix = ownerName.empty() ? "" : ownerName + "-";
	m_name = prefix + "AABBCollider";
}

void AABBCollider::SetAABB(const AABBData& aabb)
{
	m_aabb = aabb;
}

AABBData AABBCollider::GetWorldAABB() const
{
	return AABBData(m_aabb.lowerBound + m_position, m_aabb.upperBound + m_position);
}

bool AABBCollider::IsIntersecting(const Collider& other) const
{
	return other.IsIntersectAABB(*this);
}

bool AABBCollider::IsIntersectAABB(const AABBCollider& other) const
{
	AABBData self = GetWorldAABB();
	AABBData rhs = other.GetWorldAABB();
	return self.Intersecting(rhs);
}

bool AABBCollider::IsIntersectCircle(const CircleCollider& other) const
{
	return other.IsIntersectAABB(*this);
}

} // namespace LittleEngine
