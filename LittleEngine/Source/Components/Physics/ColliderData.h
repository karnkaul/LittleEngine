#pragma once
#include "Engine/CoreGame.hpp"

namespace LittleEngine {
	// \brief Wrapper for Axis Aligned Bounding Box Collider data.
	// Provides APIs to test intersection and point presence
	struct AABBData {
		static const AABBData One;

		Vector2 lowerBound;
		Vector2 upperBound;

		AABBData() = default;
		AABBData(const AABBData&) = default;
		AABBData(AABBData&&) = default;
		AABBData& operator=(const AABBData&) = default;
		AABBData& operator=(AABBData&&) = default;
		AABBData(const Vector2& lowerBound, const Vector2& upperBound);
		AABBData(const Fixed& xMax, const Fixed& yMax);

		bool Intersecting(const AABBData& other) const;
		bool IsPointInRect(const Vector2& point) const;
	};

	// \brief Wrapper for Circle Collider data.
	// Provides APIs to test intersection and point presence
	struct CircleData {
		static const CircleData One;
		
		Vector2 centre;
		Fixed radius;

		CircleData() = default;
		CircleData(const Fixed& radius, const Vector2& centre);
		CircleData(const CircleData&) = default;
		CircleData(CircleData&&) = default;
		CircleData& operator=(const CircleData&) = default;
		CircleData& operator=(CircleData&&) = default;

		bool IsIntersecting(const CircleData& other) const;
		bool IsPointInCircle(const Vector2& point) const;
	};
}
