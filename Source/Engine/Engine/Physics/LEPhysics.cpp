#include "Core/Logger.h"
#include "Core/Utils.h"
#include "Collider.h"
#include "LEPhysics.h"

namespace LittleEngine
{
namespace
{
bool IgnoreSignatures(s32 lhs, s32 rhs)
{
	return lhs != 0 && rhs != 0 && lhs == rhs;
}
} // namespace
LEPhysics::LEPhysics()
{
	LOG_D("[CollisionManager] constructed", m_logName.c_str());
}

LEPhysics::~LEPhysics()
{
	Clear();
	LOG_D("[CollisionManager] destroyed", m_logName.c_str());
}

void LEPhysics::Clear()
{
	m_colliders.clear();
}

void LEPhysics::Step(Time /*dt*/)
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

CircleCollider* LEPhysics::CreateCircleCollider(std::string ownerName)
{
	auto uCollider = MakeUnique<CircleCollider>(std::move(ownerName));
	CircleCollider* pCollider = uCollider.get();
	m_colliders.emplace_back(std::move(uCollider));
	return pCollider;
}

AABBCollider* LEPhysics::CreateAABBCollider(std::string ownerName)
{
	auto uCollider = MakeUnique<AABBCollider>(std::move(ownerName));
	AABBCollider* pCollider = uCollider.get();
	m_colliders.emplace_back(std::move(uCollider));
	return pCollider;
}

void LEPhysics::Scrub()
{
	Core::RemoveIf<UPtr<Collider>>(m_colliders, [](UPtr<Collider>& uCollider) { return uCollider->m_bDestroyed; });
}
} // namespace LittleEngine
