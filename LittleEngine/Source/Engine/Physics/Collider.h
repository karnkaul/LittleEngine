#pragma once
#include "Components/Component.h"
#include "ColliderData.h"
#include "Delegate.hpp"

namespace LittleEngine {
	class Actor;
	class CollisionManager;
	class World;

	// \brief Base class for Collision detection on an Actor.
	// Note: Registration with CollisionManager must be handled by owner
	class Collider : public Component {
	public:
		static Fixed DEBUG_BORDER_WIDTH;
		using Ptr = std::shared_ptr<Collider>;
		using wPtr = std::weak_ptr<Collider>;

		virtual ~Collider();

		// Abstract Visitor
		virtual bool IsIntersecting(const Collider& rhs) const = 0;
		virtual void DrawDebugShape(bool show, const Fixed& thickness = DEBUG_BORDER_WIDTH) = 0;
		// Covariant override for Component::SClone()
		virtual std::shared_ptr<Collider> SCloneCollider(Actor& owner) const = 0;

		void OnHit(Collider& other);

	protected:
		const World* world;
		std::shared_ptr<class ShapeRenderer> debugShape;

		Collider(Actor& actor, const std::string& name);
		
		friend class AABBCollider;
		// AABB Visitor 
		virtual bool IsIntersectAABB(const class AABBCollider& rhs) const = 0;
		
		friend class CircleCollider;
		// Circle Visitor
		virtual bool IsIntersectCircle(const class CircleCollider& rhs) const = 0;

	private:
		GameUtils::Delegate<>::Token debugOnToken;
		GameUtils::Delegate<>::Token debugOffToken;
	};

	// \brief Concrete class for 2D Circle collider
	class CircleCollider : public Collider {
	public:
		CircleCollider(Actor& actor);
		CircleCollider(Actor& owner, const CircleCollider& prototype);
		
		CircleData GetWorldCircle() const;
		void SetCircle(Fixed radius);

		virtual bool IsIntersecting(const Collider& rhs) const override;
		virtual void DrawDebugShape(bool bShow, const Fixed& thickness = DEBUG_BORDER_WIDTH) override;
		virtual void Render(RenderParams params) override;
		virtual std::shared_ptr<Collider> SCloneCollider(Actor& owner) const override;

	protected:
		friend class AABBCollider;
		virtual bool IsIntersectAABB(const AABBCollider& rhs) const override;
		virtual bool IsIntersectCircle(const CircleCollider& rhs) const override;

	private:
		CircleData circle = CircleData::One;
	};

	// \brief Concrete class for 2D AABB Collider
	class AABBCollider : public Collider {
	public:
		AABBCollider(Actor& actor);
		AABBCollider(Actor& owner, const AABBCollider& prototype);

		AABBData GetWorldAABB() const;
		void SetBounds(AABBData bounds);
		
		virtual bool IsIntersecting(const Collider& rhs) const override;
		virtual void DrawDebugShape(bool bShow, const Fixed& thickness = DEBUG_BORDER_WIDTH) override;
		virtual void Render(RenderParams params) override;
		virtual std::shared_ptr<Collider> SCloneCollider(Actor& owner) const override;

	protected:
		friend class CircleCollider;
		virtual bool IsIntersectAABB(const class AABBCollider& rhs) const override;
		virtual bool IsIntersectCircle(const class CircleCollider& rhs) const override;

	private:
		AABBData bounds = AABBData::One;
	};
}
