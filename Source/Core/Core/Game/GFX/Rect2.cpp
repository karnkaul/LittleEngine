#include "Rect2.h"
#include "../LECoreUtils/Maths.h"

namespace Core
{
const Rect2 Rect2::Zero = {Vector2::Zero, Vector2::Zero};
const Rect2 Rect2::UV = {Vector2::Zero, Vector2::One};

Rect2 Rect2::SizeCentre(Vector2 size, Vector2 centre)
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

Rect2 Rect2::SizeTL(Vector2 size, Vector2 topLeft)
{
	return {topLeft, topLeft + size};
}

Rect2 Rect2::Lerp(const Rect2& min, const Rect2& max, Fixed alpha)
{
	Vector2 tl = Maths::Lerp(min.topLeft, max.topLeft, alpha);
	Vector2 br = Maths::Lerp(min.bottomRight, max.bottomRight, alpha);
	return Rect2{tl, br};
}

Fixed Rect2::Height() const
{
	return Maths::Abs(topLeft.y - bottomRight.y);
}

Fixed Rect2::Width() const
{
	return Maths::Abs(bottomRight.x - topLeft.x);
}

Vector2 Rect2::Size() const
{
	return Vector2(Width(), Height());
}

Vector2 Rect2::Centre() const
{
	return Fixed::OneHalf * (topLeft + bottomRight);
}

Vector2 Rect2::TopLeft() const
{
	return topLeft;
}

Vector2 Rect2::TopRight() const
{
	return {bottomRight.x, topLeft.y};
}

Vector2 Rect2::BottomLeft() const
{
	return {topLeft.x, bottomRight.y};
}

Vector2 Rect2::BottomRight() const
{
	return bottomRight;
}

bool Rect2::IsPointIn(Vector2 point) const
{
	return point.x >= topLeft.x && point.x <= bottomRight.x && point.y >= bottomRight.y && point.y <= topLeft.y;
}
} // namespace Core
