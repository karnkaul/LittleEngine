#include "le_stdafx.h"
#include "World.h"
#include "Engine/Logger/Logger.h"
#include "Utils.h"

namespace LittleEngine {
	World::World(const Vector2& screenSize) : Object("World"), screenSize(screenSize) {
		Fixed halfScreenX = screenSize.x * Fixed::OneHalf;
		Fixed halfScreenY = screenSize.y * Fixed::OneHalf;
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
		return std::move(screenPoint);
	}
	
	Vector2 World::NormalisedToWorldPoint(const Vector2 & normalisedPoint, bool autoClamp) const {
		Vector2 p(normalisedPoint.x, normalisedPoint.y);
		if (autoClamp) {
			// -1 <= x, y <= 1
			p.x = Maths::Clamp_11(p.x);
			p.y = Maths::Clamp_11(p.y);
		}
		// r` = r * screen.r
		Vector2 s = screenBounds.upper;
		return Vector2(p.x * s.x, p.y * s.y);
	}
}
