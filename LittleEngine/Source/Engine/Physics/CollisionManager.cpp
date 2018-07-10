#include "stdafx.h"
#include "CollisionManager.h"
#include "Collider.h"
#include "Engine/Logger/Logger.h"

namespace Game {
	CollisionManager::CollisionManager() : Object("CollisionManager") {
	}

	void CollisionManager::Register(Collider::Ptr collider) {
		colliders.push_back(collider);
		Logger::Log(*this, "Registered new collider", Logger::Severity::Debug);
	}
	

	bool CollisionManager::Unregister(Collider::Ptr collider) {
		auto search = std::find_if(colliders.begin(), colliders.end(),
		[collider](Collider::wPtr c) {
			Collider::Ptr _c = c.lock();
			if (_c != nullptr) {
				return _c.get() == collider.get();
			}
			return false;
		}
		);
		if (search != colliders.end()) {
			colliders.erase(search);
			return true;
		}
		return false;
	}

	void CollisionManager::FixedTick() {
		Cleanup();
		Logger::Log(*this, "Processing " + std::to_string(colliders.size()) + " Colliders", Logger::Severity::HOT);
		for (auto& collider : colliders) {
			// Check collisions
		}
	}

	void CollisionManager::Cleanup() {
		int count = colliders.size();
		auto ptr = std::remove_if(colliders.begin(), colliders.end(),
			[](Collider::wPtr& collider) { return collider.lock() == nullptr; }
		);
		colliders.erase(ptr, colliders.end());

		int diff = count - colliders.size();
		if (diff > 0) {
			Logger::Log(*this, "Removed " + std::to_string(diff) + " stale Colliders ", Logger::Severity::Debug);
		}
	}
}
