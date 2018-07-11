#pragma once
#include "Object.h"
#include "Utils/Vector2.h"

namespace Game {
	class World : public Object {
	public:
		World(const Vector2& screenSize);
		~World();
		const Vector2& GetScreenSize() const { return screenSize; }
		Vector2 WorldToScreenPoint(const Vector2& worldPoint) const;
		Vector2 GetScreenBoundsX() const;
		Vector2 GetScreenBoundsY() const;

	private:
		Vector2 screenSize;
		Vector2 screenCentre;
	};
}
