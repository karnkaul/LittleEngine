#pragma once
#include "Game/Model/World/Component.h"

namespace LittleEngine
{
class CollisionComponent : public AComponent
{
public:
	static constexpr TimingType TIMING = TimingType::Last;

protected:
	struct ColliderData
	{
		Vector2 offset;
		class Collider* pCollider;
#if defined(DEBUGGING)
		class ASFDrawable* pShape = nullptr;
#endif
	};

	std::vector<ColliderData> m_pColliders;
	s32 m_signature = 0;

public:
	CollisionComponent();
	~CollisionComponent() override;

protected:
	void OnCreated() override;

public:
	void Tick(Time dt) override;

public:
	TimingType Timing() const override;
	void SetEnabled(bool bEnabled) override;

public:
	void AddCircle(Fixed diameter, Vector2 offset = Vector2::Zero);
	void AddAABB(const struct AABBData& aabbData, Vector2 offset = Vector2::Zero);
};
} // namespace LittleEngine
