#include "stdafx.h"
#include "World.h"

#include "Engine/Logger/Logger.h"
#include <sstream>

namespace Game {
	World::World(const Vector2& screenSize, const Vector2& worldSize) : Object("World") {
		this->screenSize = screenSize;
		this->worldSize = worldSize;
		worldCentre = Vector2(worldSize.x / 2, worldSize.y / 2);
		screenCentre = Vector2(this->screenSize.x / 2, this->screenSize.y / 2);
		std::stringstream ss;
		ss << "Created World " << this->worldSize.ToString() << " for Screen " << this->screenSize.ToString();
		Logger::Log(*this, ss.str());
	}

	World::~World() {
		if (worldSize.x != 0) {
			Logger::Log(*this, "World destroyed " + worldSize.ToString());
		}
	}

	Vector2 World::WorldToScreenPoint(const Vector2 & worldPoint) const {
		Vector2 screenPoint;
		screenPoint.x = worldPoint.x * (screenSize.x / worldSize.x);
		screenPoint.y = -worldPoint.y * (screenSize.y / worldSize.y);
		screenPoint += screenCentre;
		return screenPoint;
	}

	Vector2 World::ScreenToWorldPoint(const Vector2 & screenPoint) const {
		Vector2 worldPoint;
		worldPoint.x = screenPoint.x * (worldSize.x / screenSize.x);
		worldPoint.y = -screenPoint.y * (worldSize.y / screenSize.y);
		worldPoint -= worldCentre;
		return worldPoint;
	}

	Vector2 World::GetWorldBoundsX() const {
		Fixed halfWorldX = worldSize.x / 2;
		return Vector2(-halfWorldX, halfWorldX);
	}

	Vector2 World::GetWorldBoundsY() const {
		Fixed halfWorldY = worldSize.y / 2;
		return Vector2(-halfWorldY, halfWorldY);
	}
}
