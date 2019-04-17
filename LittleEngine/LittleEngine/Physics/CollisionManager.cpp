#include "stdafx.h"
#include "Logger.h"
#include "Utils.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "LittleEngine/Services/Services.h"

namespace LittleEngine
{
namespace
{
bool IgnoreSignatures(s32 lhs, s32 rhs)
{
	return lhs != 0 && rhs != 0 && lhs == rhs;
}
} // namespace
CollisionManager::CollisionManager()
{
	LOG_D("[CollisionManager] constructed", m_logName.c_str());
}

CollisionManager::~CollisionManager()
{
	m_colliders.clear();
	LOG_D("[CollisionManager] destroyed", m_logName.c_str());
}

void CollisionManager::Tick(Time /*dt*/)
{
	Scrub();

	for (size_t i = 0; i < m_colliders.size(); ++i)
	{
		auto& lhs = m_colliders[i];
		if (!lhs->m_bEnabled)
		{
			return;
		}
		for (size_t j = i + 1; j < m_colliders.size(); ++j)
		{
			auto& rhs = m_colliders[j];
			if (!rhs->m_bEnabled || IgnoreSignatures(lhs->m_ignoreSig, rhs->m_ignoreSig))
			{
				continue;
			}
			if (lhs->IsIntersecting(*rhs))
			{
				lhs->OnHit(*rhs);
				rhs->OnHit(*lhs);
			}
		}
	}
}

CircleCollider* CollisionManager::CreateCircleCollider(String ownerName)
{
	auto uCollider = MakeUnique<CircleCollider>(std::move(ownerName));
	CircleCollider* pCollider = uCollider.get();
	m_colliders.emplace_back(std::move(uCollider));
	return pCollider;
}

AABBCollider* CollisionManager::CreateAABBCollider(String ownerName)
{
	auto uCollider = MakeUnique<AABBCollider>(std::move(ownerName));
	AABBCollider* pCollider = uCollider.get();
	m_colliders.emplace_back(std::move(uCollider));
	return pCollider;
}

void CollisionManager::Scrub()
{
	Core::CleanVector<UPtr<Collider>>(
		m_colliders, [](UPtr<Collider>& uCollider) { return uCollider->m_bDestroyed; });
}
} // namespace LittleEngine
