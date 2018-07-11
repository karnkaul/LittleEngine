#include "stdafx.h"
#include "AABB.h"

const Fixed half = Fixed(1, 2);
const AABB AABB::One = AABB(Vector2(-half, -half), Vector2(half, half));

AABB::AABB(const Vector2& lowerBound, const Vector2& upperBound) : lowerBound(lowerBound), upperBound(upperBound) {
}

AABB::AABB(Fixed xMax, Fixed yMax) : AABB(Vector2(-xMax, -yMax), Vector2(xMax, yMax)) {
}

bool AABB::Intersecting(const AABB & other) {
	bool xIntersects = 
		(other.lowerBound.x >= lowerBound.x && other.lowerBound.x <= upperBound.x) ||
		(lowerBound.x >= other.lowerBound.x && lowerBound.x <= other.upperBound.x);
	bool yIntersects =
		(other.lowerBound.y >= lowerBound.y && other.lowerBound.y <= upperBound.y) ||
		(lowerBound.y >= other.lowerBound.y && lowerBound.y <= other.upperBound.y);
	return xIntersects && yIntersects;
}
