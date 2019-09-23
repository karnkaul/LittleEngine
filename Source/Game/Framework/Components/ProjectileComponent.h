#pragma once
#include "Model/World/Component.h"

namespace LittleEngine
{
class ProjectileComponent : public AComponent
{
private:
	Vector2 m_direction = Vector2::Right;
	Fixed m_speed = 3000;
	Time m_ttl = Time::Seconds(1.0f);
	Time m_elapsed;
	bool m_bSelfDestruct = true;

public:
	ProjectileComponent();
	~ProjectileComponent() override;

protected:
	void OnCreated() override;

public:
	void Tick(Time dt) override;

public:
	TimingType Timing() const override;
	void SetEnabled(bool bEnabled) override;

public:
	ProjectileComponent* SetSpeed(Fixed speed);
	ProjectileComponent* SetDirection(Vector2 direction);
	ProjectileComponent* SetTimeToLive(Time ttl);
	ProjectileComponent* SetSelfDestruct(bool bSelfDestruct);
};
} // namespace LittleEngine
