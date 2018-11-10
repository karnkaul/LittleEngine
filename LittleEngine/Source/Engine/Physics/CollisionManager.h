#pragma once
#include <vector>
#include "Engine/Object.h"
#include "Engine/Physics/Collider.h"
#include "Engine/Logger/Logger.h"

namespace LittleEngine {
	// \brief Handles Collider registrations and collision notifications
	class CollisionManager final : public Object {
	public:
		CollisionManager();

		bool Unregister(Collider& collider);
		void FixedTick();

	private:
		std::vector<Collider::wPtr> colliders;

		CollisionManager(const CollisionManager&) = delete;
		CollisionManager& operator=(const CollisionManager&) = delete;

		template<typename T>
		std::shared_ptr<T> CreateCollider(Actor& owner) {
			static_assert(std::is_base_of<Collider, T>::value, "T must derive from Collider: check Output window for erroneous call");
			std::shared_ptr<T> collider = std::make_shared<T>(owner);
			colliders.push_back(collider);
			Logger::Log(*this, "Registered 1 new " + collider->GetNameInBrackets() + " collider", Logger::Severity::Debug);
			return collider;
		}

		template<typename T>
		std::shared_ptr<T> CreateCollider(Actor& owner, T& prototype) {
			static_assert(std::is_base_of<Collider, T>::value, "T must derive from Collider: check Output window for erroneous call");
			std::shared_ptr<Component> sCollider = prototype.UClone(owner);
			std::shared_ptr<T> collider = std::dynamic_pointer_cast<T>(sCollider);
			colliders.push_back(collider);
			Logger::Log(*this, "Registered 1 new " + collider->GetNameInBrackets() + " collider", Logger::Severity::Debug);
			return collider;
		}
		
		void Cleanup();
		void ProcessCollision(Collider& lhs, Collider& rhs);

		friend class Actor;
	};
}
