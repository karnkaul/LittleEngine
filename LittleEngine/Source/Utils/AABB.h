#pragma once
#include "Vector2.h"

struct AABB {
public:
	const static AABB One;
	Vector2 lowerBound;
	Vector2 upperBound;

	AABB() = default;
	AABB(const Vector2& lowerBound, const Vector2& upperBound);
	AABB(Fixed xMax, Fixed yMax);

	bool Intersecting(const AABB& other);
	friend AABB operator+(const AABB& lhs, const Vector2& displacement) {
		return AABB(lhs.lowerBound + displacement, lhs.upperBound + displacement);
	}
};
