#include "ColliderData.h"

namespace LittleEngine
{
const AABBData AABBData::One = AABBData(Vector2::One);
const CircleData CircleData::One = CircleData(Fixed::One, Vector2(0, 0));

AABBData::AABBData(Vector2 lowerBound, const Vector2 upperBound) : lowerBound(std::move(lowerBound)), upperBound(std::move(upperBound)) {}

AABBData::AABBData(Vector2 size) : AABBData(-Fixed::OneHalf * size, Fixed::OneHalf * size) {}

bool AABBData::Intersecting(const AABBData& other) const
{
	// Compare coordinates directly
	bool xIntersects = (other.lowerBound.x >= lowerBound.x && other.lowerBound.x <= upperBound.x)
					   || (lowerBound.x >= other.lowerBound.x && lowerBound.x <= other.upperBound.x);
	bool yIntersects = (other.lowerBound.y >= lowerBound.y && other.lowerBound.y <= upperBound.y)
					   || (lowerBound.y >= other.lowerBound.y && lowerBound.y <= other.upperBound.y);
	return xIntersects && yIntersects;
}

bool AABBData::IsPointInRect(Vector2 point) const
{
	// Compare coordinates directly
	return point.x >= lowerBound.x && point.x <= upperBound.x && point.y >= lowerBound.y && point.y <= upperBound.y;
}

CircleData::CircleData(Fixed radius, Vector2 centre) : centre(std::move(centre)), radius(std::move(radius)) {}

bool CircleData::IsIntersecting(const CircleData& other) const
{
	// Compare radii and distance
	f64 centreDist = (other.centre - centre).SqrMagnitude();
	f64 radiiDist = (other.radius + radius).ToF64();
	return radiiDist * radiiDist >= centreDist * centreDist;
}

bool CircleData::IsPointInCircle(Vector2 point) const
{
	// Compare radii and distance
	Fixed distance = (point - centre).Magnitude();
	return distance < radius;
}
} // namespace LittleEngine
