#pragma once
#include "Engine/Object.h"
#include "Utils/Transform.h"

class Transform;

namespace Game {
	// Forward declarations
	struct RenderParams;
	class Level;
	class Component;

	// Base class representing an entity in the world
	class Actor : public Object, public std::enable_shared_from_this<Actor> {
	public:
		Actor(Level& level, std::string name);
		virtual ~Actor();

		Transform::Ptr GetTransform() const;
		virtual std::string ToString() const;

		virtual void FixedTick();
		virtual void Tick(Fixed deltaTime);
		virtual void Render(RenderParams& params);

		Level& GetActiveLevel() const;
		
		template<typename T>
		T& AddComponent() {
			static_assert(std::is_base_of<Component, T>::value, "T must derive from Component: check Output window for erroneous call");
			std::unique_ptr<T> component = std::make_unique<T>(*this);
			T* ptr = &(*component);
			components.push_back(std::move(component));
			return *ptr;
		}

		// Warning: Will return nullptr if not found
		template<typename T>
		T* GetComponent() {
			static_assert(std::is_base_of<Component, T>::value, "T must derive from Component: check Output window for erroneous call");
			for (auto& component : components) {
				Component* c = component.get();
				T* t = dynamic_cast<T*>(c);
				if (t != nullptr) return t;
			}
			return nullptr;
		}

	protected:
		std::vector<std::unique_ptr<Component> > components;
		Transform::Ptr transform;
		Level* level;
		bool _destroyed = false;

		std::shared_ptr<Actor> This();
		
	private:
		friend class Level;
	};
}
