#pragma once
#include "LEGame/Model/World/Component.h"

namespace LittleEngine
{
class CollisionComponent : public AComponent
{
public:
	static constexpr TimingType TIMING = TimingType::LAST;

protected:
	struct ColliderData
	{
		Vector2 offset;
		class Collider* pCollider;
#if DEBUGGING
		class SFPrimitive* pSFPrimitive = nullptr;
#endif

		ColliderData(Collider* pCollider, Vector2 offset);
#if DEBUGGING
		ColliderData(Collider* pCollider, SFPrimitive* pSFPrimitive, Vector2 offset);
#endif
	};

	Vec<ColliderData> m_pColliders;
	s32 m_signature = 0;

public:
	~CollisionComponent() override;

	void OnCreated() override;

	void AddCircle(Fixed radius, Vector2 offset = Vector2::Zero);
	void AddAABB(const struct AABBData& aabbData, Vector2 offset = Vector2::Zero);

	TimingType GetComponentTiming() const override;
	void Tick(Time dt) override;
	void SetEnabled(bool bEnabled) override;
};
} // namespace LittleEngine
