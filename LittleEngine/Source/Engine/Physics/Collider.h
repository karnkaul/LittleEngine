#pragma once
#include "Components/Component.h"
#include "AABB.h"

namespace Game {
	class Actor;
	class CollisionManager;

	class Collider : public Component {
	public:
		using Ptr = std::shared_ptr<Collider>;
		using wPtr = std::weak_ptr<Collider>;
		virtual bool IsIntersecting(const Collider& rhs) const = 0;
	protected:
		Collider(Actor& actor, std::string name);
		friend class AABBCollider;
		virtual bool IsIntersectAABB(const class AABBCollider& rhs) const = 0;
		friend class CircleCollider;
		virtual bool IsIntersectCircle(const class CircleCollider& rhs) const = 0;
	};

	class CircleCollider : public Collider {
	public:
		CircleCollider(Actor& actor);
		virtual bool IsIntersecting(const Collider& rhs) const override;
		Fixed GetRadius() const;
		void SetRadius(Fixed radius);
	protected:
		friend class AABBCollider;
		virtual bool IsIntersectAABB(const AABBCollider& rhs) const override;
		virtual bool IsIntersectCircle(const CircleCollider& rhs) const override;
	private:
		Fixed radius = Fixed::One;
	};

	class AABBCollider : public Collider {
	public:
		AABBCollider(Actor& actor);
		virtual bool IsIntersecting(const Collider& rhs) const override;
		AABB GetWorldAABB() const;
		void SetBounds(AABB bounds);
	protected:
		friend class CircleCollider;
		virtual bool IsIntersectAABB(const class AABBCollider& rhs) const override;
		virtual bool IsIntersectCircle(const class CircleCollider& rhs) const override;
	private:
		AABB bounds = AABB::One;
	};
}
