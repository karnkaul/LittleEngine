#pragma once
#include "Core/CoreTypes.h"

namespace LittleEngine
{
class LEPhysics final
{
private:
	std::vector<UPtr<class Collider>> m_colliders;
	std::string m_logName;

public:
	LEPhysics();
	~LEPhysics();

	class CircleCollider* CreateCircleCollider(std::string ownerName);
	class AABBCollider* CreateAABBCollider(std::string ownerName);

private:
	void Clear();
	void Step(Time fdt);

	void Scrub();

private:
	friend class GameManager;
};
} // namespace LittleEngine
