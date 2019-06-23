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
	static Rect2 CentreSize(Vector2 size, Vector2 centre = Vector2::Zero);
	static Rect2 BLTR(Vector2 bottomLeft, Vector2 topRight);
	static Rect2 TLBR(Vector2 topLeft, Vector2 bottomRight);
	static Rect2 TLSize(Vector2 size, Vector2 topLeft);

	Rect2() = default;
	Rect2(const Rect2&) = default;
	Rect2(Rect2&&) = default;
	Rect2& operator=(const Rect2&) = default;
	Rect2& operator=(Rect2&&) = default;

	Fixed GetHeight() const;
	Fixed GetWidth() const;
	Vector2 GetSize() const;
	Vector2 GetCentre() const;
	Vector2 GetTopLeft() const;
	Vector2 GetTopRight() const;
	Vector2 GetBottomLeft() const;
	Vector2 GetBottomRight() const;

	bool IsPointIn(Vector2 point) const;
};
} // namespace Core
