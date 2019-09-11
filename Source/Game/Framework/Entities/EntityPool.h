#pragma once
#include "Model/Inheritable.h"

namespace LittleEngine
{
class EntityPool : public Inheritable
{
protected:
	Vec<class Entity*> m_pool;
	std::function<Entity*(u16 id)> m_spawnNew;

public:
	EntityPool(String name);
	~EntityPool() override;

	void Init(std::function<Entity*(u16 id)> spawnNew, u16 count = 1);
	void Add(Entity& entity, bool bDespawn = true);

	Entity* New(Vector2 position = Vector2::Zero, Vector2 orientation = Vector2::Right);

	u32 Size() const;
	u32 DespawnedCount() const;
};
} // namespace LittleEngine
