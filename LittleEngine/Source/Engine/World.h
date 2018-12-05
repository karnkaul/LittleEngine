#pragma once
#include "Object.h"
#include "Rect2.h"

namespace LittleEngine {
	using Fixed = GameUtils::Fixed;
	using Vector2 = GameUtils::Vector2;
	using Rect2 = GameUtils::Rect2;

	// \brief Wrapper for Screen Size
	class World : public Object {
	private:
		Rect2 m_screenBounds;
		Vector2 m_screenSize;
		Vector2 m_screenCentre;

	public:
		World(const Vector2& screenSize);
		~World();

		Vector2 GetScreenSize() const { return m_screenSize; }
		Rect2 GetScreenBounds() const { return m_screenBounds; }
		Vector2 WorldToScreenPoint(const Vector2& worldPoint) const;
		Vector2 NormalisedToWorldPoint(const Vector2& normalisedPoint, bool autoClamp = true) const;
		Fixed WorldToScreenRotation(const Fixed& worldRot) const;
	};
}
