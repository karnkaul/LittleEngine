#pragma once
#include "Vector2.h"

namespace Core
{
// \brief Specifies an axis-aligned rectangle via bottom-left and top-right points
struct Rect2
{
public:
	Vector2 topLeft;
	Vector2 bottomRight;

public:
	static const Rect2 Zero;
	static const Rect2 UV;
	static Rect2 SizeCentre(Vector2 size, Vector2 centre = Vector2::Zero);
	static Rect2 BLTR(Vector2 bottomLeft, Vector2 topRight);
	static Rect2 TLBR(Vector2 topLeft, Vector2 bottomRight);
	static Rect2 SizeTL(Vector2 size, Vector2 topLeft);
	static Rect2 Lerp(const Rect2& min, const Rect2& max, Fixed alpha);

	Rect2() = default;
	Rect2(const Rect2&) = default;
	Rect2(Rect2&&) = default;
	Rect2& operator=(const Rect2&) = default;
	Rect2& operator=(Rect2&&) = default;

	Fixed Height() const;
	Fixed Width() const;
	Vector2 Size() const;
	Vector2 Centre() const;
	Vector2 TopLeft() const;
	Vector2 TopRight() const;
	Vector2 BottomLeft() const;
	Vector2 BottomRight() const;

	bool IsPointIn(Vector2 point) const;
};
} // namespace Core
