#include "le_stdafx.h"
#include "Player.h"
#include "Levels/Level.h"
#include "Components/ControllerComponent.h"
#include "Components/RenderComponent.h"

namespace LittleEngine {
	void Player::InitPlayer(Level& level, TextureAsset& mainTexture, const AABBData& colliderBounds) {
		this->m_pLevel = &level;

		AddComponent<ControllerComponent>();

		auto renderer = AddComponent<RenderComponent>();
		renderer->SetSpriteRenderable(mainTexture);
		renderer->SetLayer(LayerID::Player);
		
		auto collider = AddCollider<AABBCollider>();
		collider->SetBounds(colliderBounds);
	}
}
