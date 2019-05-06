#include "stdafx.h"
#include "Rect2.h"

namespace Core
{
const Rect2 Rect2::Zero = Rect2::BLTR(Vector2::Zero, Vector2::Zero);

Rect2 Rect2::CentreSize(Vector2 size, Vector2 centre)
{
	Rect2 ret;
	Vector2 halfSize = Fixed::OneHalf * size;
	ret.bottomLeft = centre - halfSize;
	ret.topRight = centre + halfSize;
	return ret;
}

Rect2 Rect2::BLTR(Vector2 bottomLeft, Vector2 topRight)
{
	Rect2 ret;
	ret.bottomLeft = bottomLeft;
	ret.topRight = topRight;
	return ret;
}

Rect2 Rect2::TLSize(Vector2 size, Vector2 topLeft)
{
	Rect2 ret;
	ret.bottomLeft = Vector2(topLeft.x, topLeft.y - size.y);
	ret.topRight = Vector2(topLeft.x + size.x, topLeft.y);
	return ret;
}

Fixed Rect2::GetHeight() const
{
	return topRight.y - bottomLeft.y;
}

Fixed Rect2::GetWidth() const
{
	return topRight.x - bottomLeft.x;
}

Vector2 Rect2::GetSize() const
{
	return Vector2(GetWidth(), GetHeight());
}

Vector2 Rect2::GetCentre() const
{
	return Fixed::OneHalf * (bottomLeft + topRight);
}

Vector2 Rect2::GetTopLeft() const
{
	return Vector2(bottomLeft.x, topRight.y);
}

Vector2 Rect2::GetTopRight() const
{
	return topRight;
}

Vector2 Rect2::GetBottomLeft() const
{
	return bottomLeft;
}

Vector2 Rect2::GetBottomRight() const
{
	return Vector2(topRight.x, bottomLeft.y);
}

} // namespace Core
