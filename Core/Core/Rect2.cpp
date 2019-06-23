#include "stdafx.h"
#include "Rect2.h"
#include "Maths.h"

namespace Core
{
const Rect2 Rect2::Zero = {Vector2::Zero, Vector2::Zero};
const Rect2 Rect2::UV = {Vector2::Zero, Vector2::One};

Rect2 Rect2::CentreSize(Vector2 size, Vector2 centre)
{
	Vector2 halfSize = Fixed::OneHalf * size;
	Vector2 tl = centre + Vector2(-halfSize.x, halfSize.y);
	Vector2 br = centre + Vector2(halfSize.x, -halfSize.y);
	return {tl, br};
}

Rect2 Rect2::BLTR(Vector2 bottomLeft, Vector2 topRight)
{
	Vector2 tl(bottomLeft.x, topRight.y);
	Vector2 br(topRight.x, bottomLeft.y);
	return {tl, br};
}

Rect2 Rect2::TLBR(Vector2 topLeft, Vector2 bottomRight)
{
	return {topLeft, bottomRight};
}

Rect2 Rect2::TLSize(Vector2 size, Vector2 topLeft)
{
	return {topLeft, topLeft + size};
}

Fixed Rect2::GetHeight() const
{
	return Maths::Abs(topLeft.y - bottomRight.y);
}

Fixed Rect2::GetWidth() const
{
	return Maths::Abs(bottomRight.x - topLeft.x);
}

Vector2 Rect2::GetSize() const
{
	return Vector2(GetWidth(), GetHeight());
}

Vector2 Rect2::GetCentre() const
{
	return Fixed::OneHalf * (topLeft + bottomRight);
}

Vector2 Rect2::GetTopLeft() const
{
	return topLeft;
}

Vector2 Rect2::GetTopRight() const
{
	return {bottomRight.x, topLeft.y};
}

Vector2 Rect2::GetBottomLeft() const
{
	return {topLeft.x, bottomRight.y};
}

Vector2 Rect2::GetBottomRight() const
{
	return bottomRight;
}

bool Rect2::IsPointIn(Vector2 point) const
{
	return point.x >= topLeft.x && point.x <= bottomRight.x && point.y >= bottomRight.y &&
		   point.y <= topLeft.y;
}
} // namespace Core
