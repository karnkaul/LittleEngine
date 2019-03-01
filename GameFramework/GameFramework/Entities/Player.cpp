#include "stdafx.h"
#include "Player.h"
#include "GameFramework/Components/Components.h"

namespace LittleEngine
{
PlayerCollider::PlayerCollider(const AABBData& bounds, const Vector2& offset)
	: bounds(bounds), offset(offset)
{
}

PlayerData::PlayerData(TextureAsset& mainTexture, InitList<PlayerCollider> colliders)
	: pMainTexture(&mainTexture)
{
	for (auto& aabbData : colliders)
	{
		this->colliders.push_back(aabbData);
	}
}

PlayerData::PlayerData(TextureAsset& mainTexture, const Vector<PlayerCollider>& colliders)
	: pMainTexture(&mainTexture), colliders(colliders)
{
}

Player::Player(const String& name) : Entity(name)
{
	SetName(name, "Player");
}

void Player::InitPlayer(const PlayerData& data)
{
	auto pRenderComponent = AddComponent<RenderComponent>();
	LayerID layer = static_cast<LayerID>(LAYER_LIVE + 5);
	pRenderComponent->m_pSFPrimitive->SetTexture(*data.pMainTexture)->SetLayer(layer);

	auto pCollisionComponent = AddComponent<CollisionComponent>();
	for (auto& collider : data.colliders)
	{
		pCollisionComponent->AddAABB(collider.bounds, collider.offset);
	}
	AddComponent<ControllerComponent>();

	LOG_I("%s %s", LogNameStr(), " Initialised");
}
} // namespace LittleEngine
