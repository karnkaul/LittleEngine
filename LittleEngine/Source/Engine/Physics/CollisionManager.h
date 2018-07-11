#pragma once
#include <vector>
#include "Engine/Object.h"
#include "Engine/Physics/Collider.h"

namespace Game {
	class CollisionManager final : public Object {
	public:
		CollisionManager();
		void Register(Collider::Ptr collider);
		bool Unregister(Collider::Ptr collider);
		void FixedTick();
	private:
		CollisionManager(const CollisionManager&) = delete;
		CollisionManager& operator=(const CollisionManager&) = delete;
		std::vector<Collider::wPtr> colliders;
		void Cleanup();
		void ProcessCollision(Collider& lhs, Collider& rhs);
	};
}
