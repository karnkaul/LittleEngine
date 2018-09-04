#pragma once
#include <vector>
#include "Engine/Object.h"
#include "Engine/Physics/Collider.h"

namespace LittleEngine {
	// \brief Handles Collider registrations and collision notifications
	class CollisionManager final : public Object {
	public:
		CollisionManager();
		
		void Register(Collider::Ptr collider);
		bool Unregister(Collider::Ptr collider);
		void FixedTick();

	private:
		std::vector<Collider::wPtr> colliders;

		CollisionManager(const CollisionManager&) = delete;
		CollisionManager& operator=(const CollisionManager&) = delete;
		
		void Cleanup();
		void ProcessCollision(Collider& lhs, Collider& rhs);
	};
}
