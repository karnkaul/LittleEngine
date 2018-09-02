#include "stdafx.h"
#include "World.h"
#include "Engine/Logger/Logger.h"

namespace LittleEngine {
	World::World(const Vector2& screenSize) : Object("World") {
		this->screenSize = screenSize;
		Fixed halfScreenX = screenSize.x * Fixed::Half;
		Fixed halfScreenY = screenSize.y * Fixed::Half;
		screenCentre = Vector2(halfScreenX, halfScreenY);
		screenBounds = Rect2(-screenCentre, screenCentre);
		Logger::Log(*this, "Created World for Screen " + this->screenSize.ToString());
	}

	World::~World() {
		if (screenSize.x != 0) {
			Logger::Log(*this, "World destroyed " + screenSize.ToString());
		}
	}

	Vector2 World::WorldToScreenPoint(const Vector2 & worldPoint) const {
		Vector2 screenPoint(worldPoint.x, -worldPoint.y);
		screenPoint += screenCentre;
		return screenPoint;
	}
}
