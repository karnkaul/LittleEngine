#include "stdafx.h"
#include "Player.h"
#include "Levels/Level.h"
#include "Components/ControllerComponent.h"
#include "Components/RenderComponent.h"

namespace LittleEngine {
	Player::Player(Level& level, const std::string & texturePath, const AABBData & colliderBounds, const Vector2 & position, const Fixed & rotation) : Actor(level, "Player", position, rotation) {
		AddComponent<ControllerComponent>();
		auto renderer = AddComponent<RenderComponent>();
		renderer->SetSpriteRenderer("Assets/Ship.png");
		renderer->SetLayer(LayerID::Player);
		auto collider = AddCollider<AABBCollider>();
		collider->SetBounds(colliderBounds);
	}
}
