#include "stdafx.h"
#include "ColliderData.h"

const Fixed half = Fixed(1, 2);
const AABBData AABBData::One = AABBData(Vector2(-half, -half), Vector2(half, half));

AABBData::AABBData(const Vector2& lowerBound, const Vector2& upperBound) : lowerBound(lowerBound), upperBound(upperBound) {
}

AABBData::AABBData(Fixed xMax, Fixed yMax) : AABBData(Vector2(-xMax, -yMax), Vector2(xMax, yMax)) {
}

bool AABBData::Intersecting(const AABBData & other) {
	bool xIntersects = 
		(other.lowerBound.x >= lowerBound.x && other.lowerBound.x <= upperBound.x) ||
		(lowerBound.x >= other.lowerBound.x && lowerBound.x <= other.upperBound.x);
	bool yIntersects =
		(other.lowerBound.y >= lowerBound.y && other.lowerBound.y <= upperBound.y) ||
		(lowerBound.y >= other.lowerBound.y && lowerBound.y <= other.upperBound.y);
	return xIntersects && yIntersects;
}
