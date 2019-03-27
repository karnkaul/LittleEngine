#pragma once
#include "CoreTypes.h"
#include "SimpleTime.h"

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

	class CircleCollider* CreateCircleCollider(const String& ownerName);
	class AABBCollider* CreateAABBCollider(const String& ownerName);

#if DEBUGGING
	void ToggleDebugShapes(bool bShow);
#endif

private:
	void Scrub();
};
} // namespace LittleEngine
