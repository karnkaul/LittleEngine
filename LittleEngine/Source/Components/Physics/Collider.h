#pragma once
#include "Components/Component.h"
#include "ColliderData.h"
#include "Delegate.hpp"

namespace LittleEngine {
	class Actor;
	class CollisionManager;
	class ShapeRenderable;

	// \brief Base class for Collision detection on an Actor.
	// Note: Registration with CollisionManager must be handled by owner
	class Collider : public Component {
	public:
		using Ptr = std::shared_ptr<Collider>;
		using wPtr = std::weak_ptr<Collider>;
	
	public:
		static Fixed DEBUG_BORDER_WIDTH;
	private:
		static bool m_bShowingDebugShapes;
	
	protected:
		std::unique_ptr<ShapeRenderable> m_debugShape;
	private:
		GameUtils::Delegate<>::Token m_debugOnToken;
		GameUtils::Delegate<>::Token m_debugOffToken;

	public:
		virtual ~Collider();

		void OnHit(Collider& other);

		// Abstract Visitor
		virtual bool IsIntersecting(const Collider& rhs) const = 0;
		virtual void DrawDebugShape(bool show, const Fixed& thickness = DEBUG_BORDER_WIDTH) = 0;

	protected:
		Collider(Actor& actor, const std::string& name);
		Collider(Actor& owner, const Collider& prototype);

		bool IsShowingDebugShape() const;
		
		// AABB Visitor 
		virtual bool IsIntersectAABB(const class AABBCollider& rhs) const = 0;
		// Circle Visitor
		virtual bool IsIntersectCircle(const class CircleCollider& rhs) const = 0;

	private:
		void RegisterSubscribers();

		friend class AABBCollider;
		friend class CircleCollider;
	};

	// \brief Concrete class for 2D Circle collider
	class CircleCollider : public Collider {
	private:
		CircleData m_circle = CircleData::One;

	public:
		CircleCollider(Actor& actor);
		CircleCollider(Actor& owner, const CircleCollider& prototype);
		
		CircleData GetWorldCircle() const;
		void SetCircle(Fixed radius);

		virtual bool IsIntersecting(const Collider& rhs) const override;
		virtual void DrawDebugShape(bool bShow, const Fixed& thickness = DEBUG_BORDER_WIDTH) override;
		virtual void Render(const RenderParams& params) override;
		virtual Component::Ptr UClone(Actor& owner) const override;

	protected:
		virtual bool IsIntersectAABB(const AABBCollider& rhs) const override;
		virtual bool IsIntersectCircle(const CircleCollider& rhs) const override;

		friend class AABBCollider;
	};

	// \brief Concrete class for 2D AABB Collider
	class AABBCollider : public Collider {
	private:
		AABBData m_bounds = AABBData::One; 
	
	public:
		AABBCollider(Actor& actor);
		AABBCollider(Actor& owner, const AABBCollider& prototype);

		AABBData GetWorldAABB() const;
		void SetBounds(AABBData bounds);
		
		virtual bool IsIntersecting(const Collider& rhs) const override;
		virtual void DrawDebugShape(bool bShow, const Fixed& thickness = DEBUG_BORDER_WIDTH) override;
		virtual void Render(const RenderParams& params) override;
		virtual Component::Ptr UClone(Actor& owner) const override;

	protected:
		virtual bool IsIntersectAABB(const class AABBCollider& rhs) const override;
		virtual bool IsIntersectCircle(const class CircleCollider& rhs) const override;

		friend class CircleCollider;
	};
}
