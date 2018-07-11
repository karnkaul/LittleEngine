#pragma once
#include "Components/Component.h"
#include "Utils/AABB.h"

namespace Game {
	class Actor;
	class CollisionManager;

	class Collider : public Component {
	public:
		using Ptr = std::shared_ptr<Collider>;
		using wPtr = std::weak_ptr<Collider>;
		Collider(Actor& actor);
		AABB GetWorldAABB() const;
		void SetBounds(AABB bounds);
	private:
		AABB bounds = AABB::One;
		CollisionManager & GetCollisionManager();
	};
}
