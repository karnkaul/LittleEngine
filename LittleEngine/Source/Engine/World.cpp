#include "le_stdafx.h"
#include "World.h"
#include "Engine/Logger/Logger.h"
#include "Utils.h"

namespace LittleEngine {
	World::World(const Vector2& screenSize) : Object("World"), m_screenSize(screenSize) {
		Fixed halfScreenX = screenSize.x * Fixed::OneHalf;
		Fixed halfScreenY = screenSize.y * Fixed::OneHalf;
		m_screenCentre = Vector2(halfScreenX, halfScreenY);
		m_screenBounds = Rect2(-m_screenCentre, m_screenCentre);
		Logger::Log(*this, "Created World for Screen " + this->m_screenSize.ToString());
	}

	World::~World() {
		if (m_screenSize.x != 0) {
			Logger::Log(*this, "World destroyed " + m_screenSize.ToString());
		}
	}

	Vector2 World::WorldToScreenPoint(const Vector2 & worldPoint) const {
		Vector2 screenPoint(worldPoint.x, -worldPoint.y);
		screenPoint += m_screenCentre;
		return screenPoint;
	}
	
	Vector2 World::NormalisedToWorldPoint(const Vector2 & normalisedPoint, bool autoClamp) const {
		Vector2 p(normalisedPoint.x, normalisedPoint.y);
		if (autoClamp) {
			// -1 <= x, y <= 1
			p.x = Maths::Clamp_11(p.x);
			p.y = Maths::Clamp_11(p.y);
		}
		// r` = r * screen.r
		Vector2 s = m_screenBounds.upper;
		return Vector2(p.x * s.x, p.y * s.y);
	}

	Fixed World::WorldToScreenRotation(const Fixed & worldRot) const {
		return -worldRot;
	}
}
