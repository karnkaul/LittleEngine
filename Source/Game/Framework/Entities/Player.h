#pragma once
#include "Model/World/Entity.h"
#include "Engine/Physics/ColliderData.h"

namespace LittleEngine
{
struct PlayerCollider
{
	AABBData bounds;
	Vector2 offset;

	PlayerCollider(AABBData bounds, Vector2 offset = Vector2::Zero);
};

struct PlayerData
{
	class TextureAsset* pMainTexture;
	Vec<PlayerCollider> colliders;

	PlayerData(TextureAsset& mainTexture, Vec<PlayerCollider> colliders);
	PlayerData(PlayerData&&) = default;
	PlayerData& operator=(PlayerData&&) = default;
};

class Player final : public Entity
{
public:
	void OnCreated() override;

	void InitPlayer(PlayerData data);
};
} // namespace LittleEngine