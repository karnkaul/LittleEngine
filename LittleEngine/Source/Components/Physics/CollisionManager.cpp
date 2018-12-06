#include "le_stdafx.h"
#include "CollisionManager.h"
#include "Collider.h"
#include "Entities/Actor.h"
#include "Utils.h"

namespace LittleEngine {
	CollisionManager::CollisionManager() : Object("CollisionManager") {
	}

	bool CollisionManager::Unregister(Collider & collider) {
		if (!m_colliders.empty()) {
			auto search = std::find_if(m_colliders.begin(), m_colliders.end(),
			[&collider](Collider::wPtr toCompare) {
				if (auto rhs = toCompare.lock()) {
					return rhs.get() == &collider;
				}
				return false;
			}
			);
			if (search != m_colliders.end()) {
				m_colliders.erase(search);
				Logger::Log(*this, "Unregistered 1 existing " + collider.GetNameInBrackets() + " collider", Logger::Severity::Debug);
				return true;
			}
		}
		Logger::Log("Ignored attempt to unregistered untracked collider: " + collider.GetNameInBrackets(), Logger::Severity::Info);
		return false;
	}

	void CollisionManager::FixedTick() {
		Cleanup();
		if (m_colliders.size() < 2) return;
		for (size_t i = 0; i < m_colliders.size(); ++i) {
			auto lhs = m_colliders[i].lock();
			if (lhs != nullptr) {
				for (size_t j = i + 1; j < m_colliders.size(); ++j) {
					auto rhs = m_colliders[j].lock();
					if (rhs == nullptr) continue;
					ProcessCollision(*lhs, *rhs);
				}
			}
		}
	}

	void CollisionManager::Cleanup() {
		int count = static_cast<int>(m_colliders.size());
		GameUtils::EraseNullWeakPtrs<Collider>(m_colliders);
		int diff = count - static_cast<int>(m_colliders.size());
		if (diff > 0) {
			Logger::Log(*this, "Removed " + Strings::ToString(diff) + " stale Colliders ", Logger::Severity::Debug);
		}
	}

	void CollisionManager::ProcessCollision(Collider & lhs, Collider & rhs) {
		if (lhs.IsIntersecting(rhs)) {
			lhs.OnHit(rhs);
			rhs.OnHit(lhs);
		}
	}
}
