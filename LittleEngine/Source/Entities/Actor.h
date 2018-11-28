#pragma once
#include "Engine/Object.h"
#include "TokenHandler.hpp"
#include "Engine/Input/InputHandler.h"
#include "Transform.h"
#include "Engine/Physics/Collider.h"

namespace LittleEngine {
	struct RenderParams;
	class Level;
	class Component;

	using Transform = GameUtils::Transform;

	// \brief Base class representing a renderable entity in the world
	class Actor : public Object {
	public:
		using Ptr = std::unique_ptr<Actor>;
		const static std::string UNNAMED_ACTOR;

		Actor();

	private:
		// Each Actor must be owned by an active Level,
		// and be passed a reference to it in the constructor
		void InitActor(Level& level, int actorID, const std::string& name, const Vector2& position, const Fixed& rotation);
		// Level's CloneActor uses copy constructors of this signature (required for Actor derived classes)
		void InitActor(Level& owner, int actorID, const Actor& prototype);

		// TODO: Make private
	protected:
		void GeneralInit(Level& level, int actorID, const std::string& name);

	public:
		// For subclassing Actor
		virtual ~Actor();

		// x, y E [-1, 1] where -1/+1: edge of screen, 0: centre
		// Note: Undefined behaviour if Transform is parented
		void SetNormalisedPosition(Vector2 localNPosition);
		// Call this to Destroy this Actor
		void Destruct();
		// Call this to enable/disable Actor in the Level (useful for prototypes)
		void ToggleActive(bool enable);

		// Every Actor must always be owned by a Level
		Level& GetActiveLevel() const;
		int GetActorID() const;
		
		Transform& GetTransform() { return transform; }
		const Transform& GetTransform() const { return transform; }

		virtual std::string ToString() const override;
		
		// Add object of T : Component to this Actor; except T : Collider
		// An actor can contain N components
		template<typename T>
		T* AddComponent() {
			static_assert(std::is_base_of<Component, T>::value, "T must derive from Component: check Output window for erroneous call");
			static_assert(!std::is_base_of<Collider, T>::value, "Colliders must be added via AddCollider<T>(): check Output window for erroneous call");
			std::unique_ptr<T> component = std::make_unique<T>(*this);
			T* pComponent = component.get();
			components.push_back(std::move(component));
			return pComponent;
		}

		// Warning: Will return nullptr if not found!
		// Note: Heavyweight function
		template<typename T>
		T* GetComponent() {
			static_assert(std::is_base_of<Component, T>::value, "T must derive from Component: check Output window for erroneous call");
			static_assert(!std::is_base_of<Collider, T>::value, "Colliders must be obtained via GetCollider<T>(): check Output window for erroneous call");
			for (auto& component : components) {
				T* c = dynamic_cast<T*>(component.get());
				if (c) {
					return c;
				}
			}
			return nullptr;
		}

		// Add object of T : Collider to this Actor; only one Collider supported
		template<typename T>
		T* AddCollider() {
			static_assert(std::is_base_of<Collider, T>::value, "T must derive from Collider: check Output window for erroneous call");
			collider = GetActiveLevel().GetCollisionManager().CreateCollider<T>(*this);
			return dynamic_cast<T*>(collider.get());
		}

		// Note: Incurs runtime cast
		template<typename T>
		T* GetCollider() {
			static_assert(std::is_base_of<Collider, T>::value, "T must derive from Collider: check Output window for erroneous call");
			return dynamic_cast<T*>(collider.get());
		}

	protected:
		std::vector<Component::Ptr> components;
		Collider::Ptr collider;
		Transform transform;
		Level* level;
		bool _bDestroyed = false;
		bool _bEnabled = true;
		
		virtual void FixedTick();
		virtual void Tick(const Fixed& deltaTime);
		virtual void Render(RenderParams& params);

		// \brief Registers corresponding input scoped to Actor's lifetime
		void RegisterScopedInput(const GameInput& gameInput, OnInput::Callback callback, const OnKey& type, bool consume = false);

	private:
		int actorID = 0;
		GameUtils::TokenHandler<OnInput::Token> tokenHandler;

		Actor& operator=(const Actor&) = delete;

		friend class Level;
	};
}
