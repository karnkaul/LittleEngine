#pragma once
#include "Components/Component.h"

namespace Game {
	class Actor;
	class CollisionManager;

	class Collider : public Component {
	public:
		using Ptr = std::shared_ptr<Collider>;
		using wPtr = std::weak_ptr<Collider>;
		virtual ~Collider();
	protected:
		Collider(Actor& actor, const std::string& name);
		CollisionManager & GetCollisionManager();
	};

	class BoxCollider : public Collider {
	public:
		BoxCollider(Actor& actor);
	};
}
