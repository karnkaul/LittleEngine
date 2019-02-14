#pragma once
#include "Vector2.h"

namespace Core
{
// \brief Specifies an axis-aligned rectangle via bottom-left and top-right points
struct Rect2
{
public:
	Vector2 bottomLeft;
	Vector2 topRight;

public:
	// Constructs Rect via Centre and Size (w x h)
	static Rect2 CentreSize(const Vector2& size, const Vector2& centre = Vector2::Zero);
	// Constructs Rect via Bottom-Left and Top-Right points
	static Rect2 BLTR(const Vector2& bottomLeft, const Vector2& topRight);
	// Constructs Rect via Top-Left and Size (w x h)
	static Rect2 TLSize(const Vector2& size, const Vector2& topLeft);

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
};
} // namespace Core
