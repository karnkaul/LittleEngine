#include "Core/LECoreGame/LECoreUtils/Logger.h"
#include "SFMLAPI/Rendering/LayerID.h"
#include "SFMLAPI/System/Assets.h"
#include "Model/GameManager.h"
#include "Player.h"
#include "Framework/Components/Components.h"

namespace LittleEngine
{
PlayerCollider::PlayerCollider(AABBData bounds, Vector2 offset) : bounds(std::move(bounds)), offset(std::move(offset)) {}

PlayerData::PlayerData(TextureAsset& mainTexture, std::vector<PlayerCollider> colliders)
	: pMainTexture(&mainTexture), colliders(std::move(colliders))
{
}

void Player::OnCreated()
{
	SetType("Player");
}

void Player::InitPlayer(PlayerData data)
{
	auto pRenderComponent = AddComponent<RenderComponent>();
	auto layer = static_cast<LayerID>(ToS32(LayerID::Live) + 5);
	pRenderComponent->SetSprite(*data.pMainTexture, layer);

	auto pCollisionComponent = AddComponent<CollisionComponent>();
	for (auto& collider : data.colliders)
	{
		pCollisionComponent->AddAABB(collider.bounds, collider.offset);
	}
	AddComponent<ControllerComponent>();
	m_transform.SetOrientation(Vector2::Up);

	LOG_D("%s Initialised", m_logName.c_str());
}
} // namespace LittleEngine
