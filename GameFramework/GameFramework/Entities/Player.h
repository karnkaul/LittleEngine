#pragma once
#include "LittleEngine/Game/Entity.h"
#include "LittleEngine/Physics/ColliderData.h"

namespace LittleEngine
{
struct PlayerCollider
{
	AABBData bounds;
	Vector2 offset;

	PlayerCollider(const AABBData& bounds, Vector2 offset = Vector2::Zero);
};

struct PlayerData
{
	TextureAsset* pMainTexture;
	Vec<PlayerCollider> colliders;

	PlayerData(TextureAsset& mainTexture, InitList<PlayerCollider> colliders);
	PlayerData(TextureAsset& mainTexture, const Vec<PlayerCollider>& colliders);
};

class Player final : public Entity
{
public:
	Player(String name);

	void InitPlayer(const PlayerData& data);
};
} // namespace LittleEngine