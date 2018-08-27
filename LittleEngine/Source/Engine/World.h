#pragma once
#include "Object.h"
#include "Utils/Vector2.h"
#include "Utils/Rect2.h"

namespace Game {
	class World : public Object {
	public:
		World(const Vector2& screenSize);
		~World();
		const Vector2& GetScreenSize() const { return screenSize; }
		Vector2 WorldToScreenPoint(const Vector2& worldPoint) const;
		Rect2 GetScreenBounds() const;

	private:
		Vector2 screenSize;
		Vector2 screenCentre;
	};
}
