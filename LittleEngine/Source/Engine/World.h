#pragma once
#include "Object.h"
#include "Rect2.h"

namespace LittleEngine {
	using Fixed = GameUtils::Fixed;
	using Vector2 = GameUtils::Vector2;
	using Rect2 = GameUtils::Rect2;

	// \brief Wrapper for Screen Size
	class World : public Object {
	public:
		World(const Vector2& screenSize);
		~World();

		Vector2 GetScreenSize() const { return screenSize; }
		Rect2 GetScreenBounds() const { return screenBounds; }
		Vector2 WorldToScreenPoint(const Vector2& worldPoint) const;
		Vector2 NormalisedToWorldPoint(const Vector2& normalisedPoint, bool autoClamp = true) const;
		
	private:
		Rect2 screenBounds;
		Vector2 screenSize;
		Vector2 screenCentre;
	};
}
