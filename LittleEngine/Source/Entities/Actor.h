#pragma once
#include "Engine/Object.h"
#include "Utils/Transform.h"

#include "Engine/Physics/Collider.h"

namespace LittleEngine {
	struct RenderParams;
	class Level;
	class Component;

	using Transform = Utils::Transform;

	// \brief Base class representing a renderable entity in the world
	class Actor : public Object, public std::enable_shared_from_this<Actor> {
	public:
		using Ptr = std::shared_ptr<Actor>;
		using wPtr = std::weak_ptr<Actor>;

		// Each Actor must be owned by an active Level, 
		// and be passed a reference to it in the constructor
		Actor(Level& level, std::string name);
		// For subclassing Actor, if required
		virtual ~Actor();

		// x, y E [-1, 1] where -1/+1: edge of screen, 0: centre
		// Note: Undefined behaviour if Transform is parented
		void SetNormalisedPosition(Vector2 localNPosition);
		// Call this to Destroy this Actor
		void Destruct();

		// Every Actor must always be owned by a Level
		Level& GetActiveLevel() const;
		
		Transform& GetTransform() {
			return transform;
		}

		virtual std::string ToString() const override;
		
		// Add object of T : Component to this Actor; except T : Collider
		// An actor can contain N components
		template<typename T>
		std::shared_ptr<T> AddComponent() {
			static_assert(std::is_base_of<Component, T>::value, "T must derive from Component: check Output window for erroneous call");
			static_assert(!std::is_base_of<Collider, T>::value, "Colliders must be added via AddCollider<T>(): check Output window for erroneous call");
			std::shared_ptr<T> component = std::make_shared<T>(*this);
			std::shared_ptr<Component> _component = std::static_pointer_cast<Component>(component);
			components.push_back(_component);
			return component;
		}

		// Warning: Will return nullptr if not found!
		// Note: Heavyweight function
		template<typename T>
		std::shared_ptr<T> GetComponent() {
			static_assert(std::is_base_of<Component, T>::value, "T must derive from Component: check Output window for erroneous call");
			static_assert(!std::is_base_of<Collider, T>::value, "Colliders must be obtained via GetCollider<T>(): check Output window for erroneous call");
			for (auto& component : components) {
				std::shared_ptr<T> c = std::dynamic_pointer_cast<T>(component);
				if (c != nullptr) {
					return c;
				}
			}
			return nullptr;
		}

		// Add object of T : Collider to this Actor; only one Collider supported
		template<typename T>
		std::shared_ptr<T> AddCollider() {
			static_assert(std::is_base_of<Collider, T>::value, "T must derive from Collider: check Output window for erroneous call");
			std::shared_ptr<T> collider = std::make_shared<T>(*this);
			this->collider = std::static_pointer_cast<Collider>(collider);
			GetActiveLevel().GetCollisionManager().Register(this->collider);
			return collider;
		}

		// Note: Incurs runtime cast
		template<typename T>
		std::shared_ptr<T> GetCollider() {
			static_assert(std::is_base_of<Collider, T>::value, "T must derive from Collider: check Output window for erroneous call");
			if (collider != nullptr) {
				std::shared_ptr<T> ret = std::dynamic_pointer_cast<T>(collider);
				return ret;
			}
			return nullptr;
		}

	protected:
		std::vector<std::shared_ptr<Component>> components;
		Collider::Ptr collider;
		Transform transform;
		Level* level;
		bool _destroyed = false;
		
		virtual void FixedTick();
		virtual void Tick(Fixed deltaTime);
		virtual void Render(RenderParams& params);

		friend class Level;

	private:
		Actor(const Actor&) = delete;
		Actor& operator=(const Actor&) = delete;
	};
}
