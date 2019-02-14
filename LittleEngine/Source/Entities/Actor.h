#pragma once
#include "Engine/Object.h"
#include "Engine/CoreGame.hpp"
#include "IWorldEntity.h"
#include "TokenHandler.hpp"
#include "Engine/Input/InputHandler.h"
#include "Transform.h"
#include "Components/Physics/Collider.h"

namespace LittleEngine {
	class Component;
	
	// \brief Base class representing a renderable entity in the world
	class Actor : public Object, public IWorldEntity {
	public:
		using Ptr = std::unique_ptr<Actor>;
		static const std::string UNNAMED_ACTOR;
	
	protected:
		Transform m_transform;
		std::vector<Component::Ptr> m_components;
		Collider::Ptr m_sCollider;
		class Level* m_pLevel;
		bool m_bDestroyed = false;
		bool m_bEnabled = true;
	private:
		GameUtils::TokenHandler<OnInput::Token> m_tokenHandler;
		int m_actorID = 0;
		
	public:
		Actor();
		virtual ~Actor();

		// x, y E [-1, 1] where -1/+1: edge of screen, 0: centre
		// Note: Undefined behaviour if Transform is parented
		void SetNormalisedPosition(Vector2 localNPosition);
		// Call this to Destroy this Actor
		void Destruct();
		// Call this to enable/disable Actor in the Level (useful for prototypes)
		void ToggleActive(bool bEnable);

		// Every Actor must always be owned by a Level
		Level& GetActiveLevel() const;
		int GetActorID() const;
		
		Transform& GetTransform() { return m_transform; }
		const Transform& GetTransform() const { return m_transform; }

		virtual std::string ToString() const override;
		
		// Add object of T : Component to this Actor; except T : Collider
		// An actor can contain N components
		template<typename T>
		T* AddComponent() {
			static_assert(std::is_base_of<Component, T>::value, "T must derive from Component: check Output window for erroneous call");
			static_assert(!std::is_base_of<Collider, T>::value, "Colliders must be added via AddCollider<T>(): check Output window for erroneous call");
			std::unique_ptr<T> component = std::make_unique<T>(*this);
			T* pComponent = component.get();
			m_components.push_back(std::move(component));
			return pComponent;
		}

		// Warning: Will return nullptr if not found!
		// Note: Heavyweight function
		template<typename T>
		T* GetComponent() {
			static_assert(std::is_base_of<Component, T>::value, "T must derive from Component: check Output window for erroneous call");
			static_assert(!std::is_base_of<Collider, T>::value, "Colliders must be obtained via GetCollider<T>(): check Output window for erroneous call");
			for (auto& component : m_components) {
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
			m_sCollider = GetActiveLevel().GetCollisionManager().CreateCollider<T>(*this);
			return dynamic_cast<T*>(m_sCollider.get());
		}

		// Note: Incurs runtime cast
		template<typename T>
		T* GetCollider() {
			static_assert(std::is_base_of<Collider, T>::value, "T must derive from Collider: check Output window for erroneous call");
			return dynamic_cast<T*>(m_sCollider.get());
		}

	protected:
		// \brief Registers corresponding input scoped to Actor's lifetime
		void RegisterScopedInput(const GameInput& gameInput, OnInput::Callback callback, const OnKey& type, bool consume = false);
		RenderParams GetRenderParams() const;

		virtual void FixedTick();
		virtual void Tick(const Fixed& deltaMS) override;
		virtual void Render() override;

	private:
		Actor& operator=(const Actor&) = delete;

		// Each Actor must be owned by an active Level,
		// and be passed a reference to it in the constructor
		void InitActor(Level& level, int actorID, const std::string& name, const Vector2& position, const Fixed& orientation);
		// Level's CloneActor uses copy constructors of this signature (required for Actor derived classes)
		void InitActor(Level& owner, int actorID, const Actor& prototype);

	// TODO: Make private after removing VFX
	protected:
		void GeneralInit(Level& level, int actorID, const std::string& name);

		friend class Level;
	};
}
