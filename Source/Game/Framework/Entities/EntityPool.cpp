#include "EntityPool.h"
#include "Model/World/Entity.h"

namespace LittleEngine
{
EntityPool::EntityPool(String name) : Inheritable(std::move(name), "EntityPool") {}

EntityPool::~EntityPool()
{
	for (auto pEntity : m_pool)
	{
		pEntity->m_pPool = nullptr;
		pEntity->Destruct();
	}
}

void EntityPool::Init(std::function<Entity*(u16 id)> spawnNew, u16 count /* = 1 */)
{
	m_spawnNew = std::move(spawnNew);
	for (u16 i = 0; i < count; ++i)
	{
		Entity* pEntity = m_spawnNew(i);
		if (pEntity)
		{
			Add(*pEntity);
		}
	}
}

void EntityPool::Add(Entity& entity, bool bDespawn /* = true*/)
{
	entity.m_pPool = this;
	if (bDespawn)
	{
		entity.Despawn();
	}
	m_pool.push_back(&entity);
}

Entity* EntityPool::New(Vector2 position /* = Vector2::Zero */, Vector2 orientation /* = Vector2::Right */)
{
	Entity* pRet = nullptr;
	bool bRespawn = true;
	for (auto pEntity : m_pool)
	{
		if (pEntity->GetState() == Entity::State::Despawned)
		{
			pRet = pEntity;
			break;
		}
	}
	if (!pRet)
	{
		LOG_W("%s Pool is empty!", LogNameStr());
		Assert(m_spawnNew != nullptr, "Entity Generator Procedure is null!");
		pRet = m_spawnNew(static_cast<u16>(m_pool.size()));
		if (pRet)
		{
			Add(*pRet, false);
			bRespawn = false;
		}
	}
	if (pRet)
	{
		auto& tr = pRet->m_transform;
		tr.SetPosition(position);
		tr.SetOrientation(orientation);
		if (bRespawn)
		{
			pRet->Respawn();
		}
	}
	return pRet;
}

u32 EntityPool::Size() const
{
	return static_cast<u32>(m_pool.size());
}
u32 EntityPool::DespawnedCount() const
{
	u32 despawned = 0;
	for (auto pEntity : m_pool)
	{
		if (pEntity->GetState() == Entity::State::Despawned)
		{
			++despawned;
		}
	}
	return despawned;
}
} // namespace LittleEngine
