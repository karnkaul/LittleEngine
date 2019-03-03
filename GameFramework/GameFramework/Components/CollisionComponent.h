#pragma once
#include "LittleEngine/Game/Component.h"
#include "LittleEngine/Physics/Physics.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"

namespace LittleEngine
{
class CollisionComponent : public Component
{
public:
	static constexpr TimingType TIMING = TimingType::LAST;

protected:
	struct ColliderData
	{
		Vector2 offset;
		Collider* pCollider;
#if DEBUGGING
		SFPrimitive* pSFPrimitive;
#endif

		ColliderData(Collider* pCollider, const Vector2& offset);
#if DEBUGGING
		ColliderData(Collider* pCollider, SFPrimitive* pSFPrimitive, const Vector2& offset);
#endif
	};

	Vec<ColliderData> m_pColliders;
	s32 m_signature = 0;

public:
	CollisionComponent();
	~CollisionComponent();

	void AddCircle(const Fixed& radius, const Vector2& offset = Vector2::Zero);
	void AddAABB(const AABBData& aabbData, const Vector2& offset = Vector2::Zero);

	virtual TimingType GetComponentTiming() const override;
	virtual void Tick(Time dt) override;
	virtual void SetEnabled(bool bEnabled) override;
};
}
