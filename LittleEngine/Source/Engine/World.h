#pragma once
#include "Object.h"
#include "Utils/Vector2.h"

namespace Game {
	class World : public Object {
	public:
		World(const Vector2& screenSize, const Vector2& worldSize);
		~World();
		const Vector2& GetScreenSize() const { return screenSize; }
		const Vector2& GetWorldSize() const { return worldSize; }
		Vector2 WorldToScreenPoint(const Vector2& worldPoint) const;
		Vector2 ScreenToWorldPoint(const Vector2& screenPoint) const;
		Vector2 GetWorldBoundsX() const;
		Vector2 GetWorldBoundsY() const;

	private:
		Vector2 screenSize;
		Vector2 worldSize;
		Vector2 worldCentre;
		Vector2 screenCentre;
	};
}
