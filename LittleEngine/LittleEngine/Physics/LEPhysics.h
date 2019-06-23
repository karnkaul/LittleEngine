#pragma once
#include "Core/CoreTypes.h"

namespace LittleEngine
{
class LEPhysics final
{
private:
	Vec<UPtr<class Collider>> m_colliders;
	String m_logName;

public:
	LEPhysics();
	~LEPhysics();

	void Tick(Time dt);

	class CircleCollider* CreateCircleCollider(String ownerName);
	class AABBCollider* CreateAABBCollider(String ownerName);

private:
	void Scrub();
};
} // namespace LittleEngine
