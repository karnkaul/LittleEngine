#include "le_stdafx.h"
#include "Player.h"
#include "Levels/Level.h"
#include "Components/ControllerComponent.h"
#include "Components/RenderComponent.h"

namespace LittleEngine {
	Player::Player(Level & level, const std::string& discard, const Vector2 & position, const Fixed & rotation) : Actor(level, "Player", position, rotation) {
	}

	Player::Player(Level& level, const std::string & texturePath, const AABBData & colliderBounds, const Vector2 & position, const Fixed & rotation) : Actor(level, "Player", position, rotation) {
		TextureAsset* texture = level.GetAssetManager().Load<TextureAsset>(texturePath);
		if (texture) {
			Init(*texture, colliderBounds);
		}
	}

	void Player::Init(TextureAsset& mainTexture, const AABBData& colliderBounds) {
		AddComponent<ControllerComponent>();

		auto renderer = AddComponent<RenderComponent>();
		renderer->SetSpriteRenderer(mainTexture);
		renderer->SetLayer(LayerID::Player);
		
		auto collider = AddCollider<AABBCollider>();
		collider->SetBounds(colliderBounds);
	}
}
