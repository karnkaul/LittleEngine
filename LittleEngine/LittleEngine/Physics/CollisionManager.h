#pragma once
#include "CoreTypes.h"
#include "Collider.h"
#include "SFMLAPI/System/SFTime.h"

namespace LittleEngine
{
class CollisionManager final
{
private:
	Vector<UPtr<Collider>> m_colliders;
	String m_logName;

public:
	CollisionManager(const String& worldName);
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
}
