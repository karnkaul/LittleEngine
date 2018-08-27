#pragma once
#include "Components/Component.h"
#include "ColliderData.h"

namespace Game {
	class Actor;
	class CollisionManager;
	class World;

	class Collider : public Component {
	public:
		static Fixed DEBUG_BORDER_WIDTH;
		using Ptr = std::shared_ptr<Collider>;
		using wPtr = std::weak_ptr<Collider>;
		virtual bool IsIntersecting(const Collider& rhs) const = 0;
		virtual void DrawDebugShape(bool show, Fixed thickness = DEBUG_BORDER_WIDTH) = 0;

	protected:
		const World& world;
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
		CircleData GetWorldCircle() const;
		void SetCircle(Fixed radius);
		virtual void DrawDebugShape(bool show, Fixed thickness = DEBUG_BORDER_WIDTH) override;
		virtual void Render(RenderParams& params) override;

	protected:
		friend class AABBCollider;
		virtual bool IsIntersectAABB(const AABBCollider& rhs) const override;
		virtual bool IsIntersectCircle(const CircleCollider& rhs) const override;

	private:
		CircleData circle = CircleData::One;
		std::unique_ptr<class CircleRenderer> debugCircle;
	};

	class AABBCollider : public Collider {
	public:
		AABBCollider(Actor& actor);
		virtual bool IsIntersecting(const Collider& rhs) const override;
		AABBData GetWorldAABB() const;
		void SetBounds(AABBData bounds);
		virtual void DrawDebugShape(bool show, Fixed thickness = DEBUG_BORDER_WIDTH) override;
		virtual void Render(RenderParams& params) override;

	protected:
		friend class CircleCollider;
		virtual bool IsIntersectAABB(const class AABBCollider& rhs) const override;
		virtual bool IsIntersectCircle(const class CircleCollider& rhs) const override;

	private:
		AABBData bounds = AABBData::One;
		std::unique_ptr<class RectangleRenderer> debugRect;
	};
}
