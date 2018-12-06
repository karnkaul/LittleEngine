#include "le_stdafx.h"
#include "RenderParams.h"
#include "SFMLInterface/Graphics.h"

namespace LittleEngine {
	RenderParams::RenderParams() {
		Reset();
	}

	RenderParams::RenderParams(const Vector2 & worldPosition, Fixed worldOrientation, const Vector2 & worldScale) : worldPosition(worldPosition), worldScale(worldScale), worldOrientation(worldOrientation) {}

	Vector2 RenderParams::GetScreenPosition() const {
		return Vector2(worldPosition.x, -worldPosition.y);
	}

	Fixed RenderParams::GetScreenOrientation() const {
		return -worldOrientation;
	}

	Vector2 RenderParams::GetScreenScale() const {
		return worldScale;
	}
	
	void RenderParams::Reset() {
		worldPosition = Vector2::Zero;
		worldOrientation = Fixed::Zero;
		worldScale = Vector2::One;
	}
}
