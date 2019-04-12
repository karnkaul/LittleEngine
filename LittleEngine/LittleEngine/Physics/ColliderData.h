#pragma once
#include "CoreTypes.h"

namespace LittleEngine
{
// \brief Wrapper for Axis Aligned Bounding Box Collider data.
// Provides APIs to test intersection and point presence
struct AABBData
{
	static const AABBData One;

	Vector2 lowerBound;
	Vector2 upperBound;

	AABBData() = default;
	AABBData(const AABBData&) = default;
	AABBData(AABBData&&) = default;
	AABBData& operator=(const AABBData&) = default;
	AABBData& operator=(AABBData&&) = default;
	AABBData(Vector2 size);
	AABBData(Vector2 lowerBound, Vector2 upperBound);

	bool Intersecting(const AABBData& other) const;
	bool IsPointInRect(Vector2 point) const;
};

// \brief Wrapper for Circle Collider data.
// Provides APIs to test intersection and point presence
struct CircleData
{
	static const CircleData One;

	Vector2 centre;
	Fixed radius;

	CircleData() = default;
	CircleData(Fixed radius, Vector2 centre);
	CircleData(const CircleData&) = default;
	CircleData(CircleData&&) = default;
	CircleData& operator=(const CircleData&) = default;
	CircleData& operator=(CircleData&&) = default;

	bool IsIntersecting(const CircleData& other) const;
	bool IsPointInCircle(Vector2 point) const;
};
} // namespace LittleEngine
