#pragma once
#include "Utils/Vector2.h"

struct AABBData {
	const static AABBData One;
	Vector2 lowerBound;
	Vector2 upperBound;

	AABBData() = default;
	AABBData(const Vector2& lowerBound, const Vector2& upperBound);
	AABBData(Fixed xMax, Fixed yMax);

	bool Intersecting(const AABBData& other);
	friend AABBData operator+(const AABBData& lhs, const Vector2& displacement) {
		return AABBData(lhs.lowerBound + displacement, lhs.upperBound + displacement);
	}
};

struct CircleData {
	const static CircleData One;
	Fixed radius;
	Fixed centre;

	CircleData() = default;
	CircleData(Fixed radius, Fixed centre);
};
