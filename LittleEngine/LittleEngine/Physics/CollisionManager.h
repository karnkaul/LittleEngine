#pragma once
#include "Core/CoreTypes.h"

namespace LittleEngine
{
class CollisionManager final
{
private:
	Vec<UPtr<class Collider>> m_colliders;
	String m_logName;

public:
	CollisionManager();
	~CollisionManager();

	void Tick(Time dt);

	class CircleCollider* CreateCircleCollider(String ownerName);
	class AABBCollider* CreateAABBCollider(String ownerName);

private:
	void Scrub();
};
} // namespace LittleEngine
