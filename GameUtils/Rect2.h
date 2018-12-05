#pragma once
#include "Vector2.h"

namespace GameUtils {
	// \brief Specifies an axis-aligned rectangle via bottom-left and top-right points
	struct Rect2 {
	public:
		Vector2 lower;
		Vector2 upper;

		Rect2() = default;
		Rect2(Vector2 lower, Vector2 upper) : lower(lower), upper(upper) {}
		Rect2(const Rect2&) = default;
		Rect2(Rect2&&) = default;
		Rect2& operator=(const Rect2&) = default;
		Rect2& operator=(Rect2&&) = default;

		Fixed GetHeight() const {
			return upper.y - lower.y;
		}

		Fixed GetWidth() const {
			return upper.x - lower.x;
		}
	};
}
