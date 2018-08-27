#include "stdafx.h"
#include "World.h"

#include "Engine/Logger/Logger.h"
#include <sstream>

namespace Game {
	World::World(const Vector2& screenSize) : Object("World") {
		this->screenSize = screenSize;
		screenCentre = Vector2(this->screenSize.x / 2, this->screenSize.y / 2);
		std::stringstream ss;
		ss << "Created World for Screen " << this->screenSize.ToString();
		Logger::Log(*this, ss.str());
	}

	World::~World() {
		if (screenSize.x != 0) {
			Logger::Log(*this, "World destroyed " + screenSize.ToString());
		}
	}

	Vector2 World::WorldToScreenPoint(const Vector2 & worldPoint) const {
		Vector2 screenPoint;
		screenPoint.x = worldPoint.x;
		screenPoint.y = -worldPoint.y;
		screenPoint += screenCentre;
		return screenPoint;
	}

	Rect2 World::GetScreenBounds() const {
		Fixed halfScreenX = screenSize.x * Fixed::Half;
		Fixed halfScreenY = screenSize.y * Fixed::Half;
		return Rect2(
			Vector2(-halfScreenX, -halfScreenY),
			Vector2(halfScreenX, halfScreenY)
		);
	}
}
