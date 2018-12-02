#pragma once
#include "Actor.h"
#include "SFMLInterface/Assets.h"

namespace LittleEngine {
	// \brief Actor subclass that auto-adds a Sprite, AABBCollider, and ControllerComponent
	class Player : public Actor {
	public:
		using Ptr = std::unique_ptr<Player>;

		void InitPlayer(Level& level, TextureAsset& mainTexture, const AABBData& colliderBounds);
	};
}
