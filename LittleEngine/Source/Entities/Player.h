#pragma once
#include "Actor.h"
#include "SFMLInterface/Assets.h"

namespace LittleEngine {
	// \brief Actor subclass that auto-adds a Sprite, AABBCollider, and ControllerComponent
	class Player : public Actor {
	public:
		using wPtr = std::weak_ptr<Player>;
		using Ptr = std::shared_ptr<Player>;

		Player(Level& level, const std::string& discard, const Vector2& position, const Fixed& rotation);
		// texturePath must be relative to root Asset directory
		Player(Level& level, const std::string & texturePath, const AABBData & colliderBounds, const Vector2 & position, const Fixed & rotation);

		void Init(TextureAsset& mainTexture, const AABBData& colliderBounds);
	};
}
