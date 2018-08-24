#include "stdafx.h"
#include "CollisionManager.h"
#include "Collider.h"
#include "Engine/Logger/Logger.h"
#include "Entities/Actor.h"

namespace Game {
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
		Logger::Log(*this, "Registered new collider", Logger::Severity::Debug);
	}
	

	bool CollisionManager::Unregister(Collider::Ptr collider) {
		if (!colliders.empty()) {
			ColliderComparer comparer(collider);
			auto search = std::find_if(colliders.begin(), colliders.end(), comparer);
			if (search != colliders.end()) {
				colliders.erase(search);
				return true;
			}
		}
		return false;
	}

	void CollisionManager::FixedTick() {
		Cleanup();
		if (colliders.size() < 2) return;
		Logger::Log(*this, "Processing " + std::to_string(colliders.size()) + " Colliders", Logger::Severity::HOT);
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
		int count = colliders.size();
		auto ptr = std::remove_if(colliders.begin(), colliders.end(),
			[](Collider::wPtr& collider) { 
				return collider.lock() == nullptr; 
			}
		); 
		colliders.erase(ptr, colliders.end());

		int diff = count - colliders.size();
		if (diff > 0) {
			Logger::Log(*this, "Removed " + std::to_string(diff) + " stale Colliders ", Logger::Severity::Debug);
		}
	}

	void CollisionManager::ProcessCollision(Collider & lhs, Collider & rhs) {
		static int DEBUG_skip = 0;
		if (lhs.IsIntersecting(rhs)) {
			if (++DEBUG_skip > 10) {
				Logger::Log(*this, lhs.GetActor().GetName() + " is colliding with " + rhs.GetActor().GetName(), Logger::Severity::Debug);
				DEBUG_skip = 0;
			}
		}
	}
}
