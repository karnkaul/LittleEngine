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

	class CircleCollider* CreateCircleCollider(String ownerName);
	class AABBCollider* CreateAABBCollider(String ownerName);

private:
	void Reset();
	void Tick(Time dt);

	void Scrub();

private:
	friend class GameManager;
};
} // namespace LittleEngine
