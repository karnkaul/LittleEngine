#include "stdafx.h"
#include "CollisionManager.h"
#include "Utils.h"
#include "Logger.h"

namespace LittleEngine
{
namespace
{
bool IgnoreSignatures(s32 lhs, s32 rhs)
{
	return lhs != 0 && rhs != 0 && lhs == rhs;
}
}
CollisionManager::CollisionManager(const String& worldName) : m_name("CollisionManager (" + worldName +")")
{
	LogD("[" + m_name + "] constructed");
}

CollisionManager::~CollisionManager()
{
	m_colliders.clear();
	LogD("[" + m_name + "] destroyed");
}

void CollisionManager::Tick(Time)
{
	for (size_t i = 0; i < m_colliders.size(); ++i)
	{
		Scrub();
		auto& lhs = m_colliders[i];
		if (!lhs->m_bEnabled)
			continue;
		for (size_t j = i + 1; j < m_colliders.size(); ++j)
		{
			auto& rhs = m_colliders[j];
			if (!rhs->m_bEnabled || IgnoreSignatures(lhs->m_ignoreSig, rhs->m_ignoreSig))
				continue;
			if (lhs->IsIntersecting(*rhs))
			{
				lhs->OnHit(*rhs);
				rhs->OnHit(*lhs);
			}
		}
	}
}

CircleCollider* CollisionManager::CreateCircleCollider(const String& ownerName)
{
	auto uCollider = MakeUnique<CircleCollider>(ownerName);
	CircleCollider* pCollider = uCollider.get();
	m_colliders.emplace_back(std::move(uCollider));
	return pCollider;
}

AABBCollider* CollisionManager::CreateAABBCollider(const String& ownerName)
{
	auto uCollider = MakeUnique<AABBCollider>(ownerName);
	AABBCollider* pCollider = uCollider.get();
	m_colliders.emplace_back(std::move(uCollider));
	return pCollider;
}

#if DEBUGGING
void CollisionManager::ToggleDebugShapes(bool bShow)
{
	Collider::s_bShowDebugShape = bShow;
}
#endif

void CollisionManager::Scrub()
{
	Core::CleanVector<UPtr<Collider>>(m_colliders, [](UPtr<Collider>& uCollider) { return uCollider->m_bDestroyed; });
}
} // namespace LittleEngine
