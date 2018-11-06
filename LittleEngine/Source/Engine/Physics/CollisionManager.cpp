#include "le_stdafx.h"
#include "CollisionManager.h"
#include "Collider.h"
#include "Engine/Logger/Logger.h"
#include "Entities/Actor.h"
#include "Utils.h"

namespace LittleEngine {
	struct ColliderComparer {
		ColliderComparer(Collider::Ptr lhs) : lhs(lhs) {}
		Collider::Ptr lhs;
		bool operator()(Collider::wPtr rhs) {
			Collider::Ptr _c = rhs.lock();
			if (_c != nullptr) {
				return _c.get() == lhs.get();
			}
			return false;
		}
	};

	CollisionManager::CollisionManager() : Object("CollisionManager") {
	}

	void CollisionManager::Register(Collider::Ptr collider) {
		colliders.push_back(collider);
		Logger::Log(*this, "Registered 1 new collider", Logger::Severity::Debug);
	}
	

	bool CollisionManager::Unregister(Collider::Ptr collider) {
		if (!colliders.empty()) {
			ColliderComparer comparer(collider);
			auto search = std::find_if(colliders.begin(), colliders.end(), comparer);
			if (search != colliders.end()) {
				colliders.erase(search);
				Logger::Log(*this, "Unregistered 1 existing collider", Logger::Severity::Debug);
				return true;
			}
		}
		Logger::Log("Attempt to unregistered untracked collider ignored", Logger::Severity::Info);
		return false;
	}

	void CollisionManager::FixedTick() {
		Cleanup();
		if (colliders.size() < 2) return;
		Logger::Log(*this, "Processing " + Strings::ToString(colliders.size()) + " Colliders", Logger::Severity::HOT);
		for (size_t i = 0; i < colliders.size(); ++i) {
			auto lhs = colliders[i].lock();
			if (lhs != nullptr) {
				for (size_t j = i + 1; j < colliders.size(); ++j) {
					auto rhs = colliders[j].lock();
					if (rhs == nullptr) continue;
					ProcessCollision(*lhs, *rhs);
				}
			}
		}
	}

	void CollisionManager::Cleanup() {
		int count = static_cast<int>(colliders.size());
		GameUtils::EraseNullWeakPtrs<Collider>(colliders);
		int diff = count - static_cast<int>(colliders.size());
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
