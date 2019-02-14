#include "Rect2.h"

namespace GameUtils {
	//Rect2::Rect2(const Vector2 &halfSize) : lower(-halfSize), upper(halfSize) {}
	//Rect2::Rect2(const Vector2 & lower, const Vector2 & upper) : lower(lower), upper(upper) {}

	Rect2 Rect2::CentreSize(const Vector2 & size, const Vector2 & centre) {
		Rect2 ret;
		Vector2 halfSize = Fixed::OneHalf * size;
		ret.bottomLeft = centre - halfSize;
		ret.topRight = centre + halfSize;
		return ret;
	}

	Rect2 Rect2::BLTR(const Vector2& bottomLeft, const Vector2& topRight) {
		Rect2 ret;
		ret.bottomLeft = bottomLeft;
		ret.topRight = topRight;
		return ret;
	}

	Rect2 Rect2::TLSize(const Vector2& size, const Vector2& topLeft) {
		Rect2 ret;
		ret.bottomLeft = Vector2(topLeft.x, topLeft.y - size.y);
		ret.topRight = Vector2(topLeft.x + size.x, topLeft.y);
		return ret;
	}

	Fixed Rect2::GetHeight() const {
		return topRight.y - bottomLeft.y;
	}

	Fixed Rect2::GetWidth() const {
		return topRight.x - bottomLeft.x;
	}

	Vector2 Rect2::GetSize() const {
		return Vector2(GetWidth(), GetHeight());
	}

	Vector2 Rect2::GetTopLeft() const {
		return Vector2(bottomLeft.x, topRight.y);
	}

	Vector2 Rect2::GetTopRight() const {
		return topRight;
	}

	Vector2 Rect2::GetBottomLeft() const {
		return bottomLeft;
	}

	Vector2 Rect2::GetBottomRight() const {
		return Vector2(topRight.x, bottomLeft.y);
	}

}
