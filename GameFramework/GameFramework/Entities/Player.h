#pragma once
#include "LittleEngine/Game/Entity.h"
#include "LittleEngine/Physics/ColliderData.h"

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
	TextureAsset* pMainTexture;
	Vec<PlayerCollider> colliders;

	PlayerData(TextureAsset& mainTexture, Vec<PlayerCollider> colliders);
	PlayerData(PlayerData&&) = default;
	PlayerData& operator=(PlayerData&&) = default;
};

class Player final : public Entity
{
public:
	Player(String name);

	void InitPlayer(PlayerData data);
};
} // namespace LittleEngine