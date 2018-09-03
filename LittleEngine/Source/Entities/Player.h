#pragma once
#include "Actor.h"

namespace LittleEngine {
	// \brief Actor subclass that auto-adds a Sprite, AABBCollider, and ControllerComponent
	class Player : public Actor {
	public:
		using wPtr = std::weak_ptr<Player>;
		using Ptr = std::shared_ptr<Player>;

		// texturePath must be relative to root Asset directory
		Player(Level& level, const std::string & texturePath, const AABBData & colliderBounds, const Vector2 & position, const Fixed & rotation);
	};
}
