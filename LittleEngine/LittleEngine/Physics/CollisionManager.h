#pragma once
#include "CoreTypes.h"
#include "SimpleTime.h"
#include "Collider.h"

namespace LittleEngine
{
class CollisionManager final
{
private:
	Vec<UPtr<Collider>> m_colliders;
	String m_logName;

public:
	CollisionManager();
	~CollisionManager();

	void Tick(Time dt);

	CircleCollider* CreateCircleCollider(const String& ownerName);
	AABBCollider* CreateAABBCollider(const String& ownerName);

#if DEBUGGING
	void ToggleDebugShapes(bool bShow);
#endif

private:
	void Scrub();
};
} // namespace LittleEngine
