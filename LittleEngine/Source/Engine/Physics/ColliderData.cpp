#include "le_stdafx.h"
#include "ColliderData.h"

namespace LittleEngine {
	const Fixed half = Fixed(1, 2);
	const AABBData AABBData::One = AABBData(Vector2(-half, -half), Vector2(half, half));
	const CircleData CircleData::One = CircleData(1, Vector2(0, 0));

	AABBData::AABBData(const Vector2& lowerBound, const Vector2& upperBound) : lowerBound(lowerBound), upperBound(upperBound) {
	}

	AABBData::AABBData(const Fixed& xMax, const Fixed& yMax) : AABBData(Vector2(-xMax, -yMax), Vector2(xMax, yMax)) {
	}

	bool AABBData::Intersecting(const AABBData & other) const {
		// Compare coordinates directly
		bool xIntersects =
			(other.lowerBound.x >= lowerBound.x && other.lowerBound.x <= upperBound.x) ||
			(lowerBound.x >= other.lowerBound.x && lowerBound.x <= other.upperBound.x);
		bool yIntersects =
			(other.lowerBound.y >= lowerBound.y && other.lowerBound.y <= upperBound.y) ||
			(lowerBound.y >= other.lowerBound.y && lowerBound.y <= other.upperBound.y);
		return xIntersects && yIntersects;
	}

	bool AABBData::IsPointInRect(const Vector2& point) const {
		// Compare coordinates directly
		return point.x >= lowerBound.x && point.x <= upperBound.x &&
			point.y >= lowerBound.y && point.y <= upperBound.y;
	}

	CircleData::CircleData(const Fixed& radius, const Vector2& centre) : radius(radius), centre(centre) {
	}

	bool CircleData::IsIntersecting(const CircleData & other) const {
		// Compare radii and distance
		Fixed centreDist = (other.centre - centre).Magnitude();
		Fixed radiiDist = other.radius + radius;
		return radiiDist >= centreDist;
	}

	bool CircleData::IsPointInCircle(const Vector2& point) const {
		// Compare radii and distance
		Fixed distance = (point - centre).Magnitude();
		return distance < radius;
	}
}
