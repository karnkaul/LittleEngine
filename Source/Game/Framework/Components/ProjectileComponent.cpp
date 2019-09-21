#include "ProjectileComponent.h"
#include "Model/World/Entity.h"

namespace LittleEngine
{
ProjectileComponent::ProjectileComponent() = default;
ProjectileComponent::~ProjectileComponent() = default;

void ProjectileComponent::OnCreated()
{
	SetName("Projectile");
}

void ProjectileComponent::Tick(Time dt)
{
	if (m_bEnabled)
	{
		m_elapsed += dt;
		if (m_bSelfDestruct && m_elapsed >= m_ttl)
		{
			m_pOwner->Destruct();
			return;
		}
		Vector2 pos = m_pOwner->m_transform.Position();
		Vector2 dPos = Fixed(dt.AsSeconds()) * m_speed * m_direction;
		pos += dPos;
		m_pOwner->m_transform.SetPosition(pos);
		m_pOwner->m_transform.SetOrientation(m_direction);
	}
}

TimingType ProjectileComponent::Timing() const
{
	return TimingType::Pre;
}

void ProjectileComponent::SetEnabled(bool bEnabled)
{
	AComponent::SetEnabled(bEnabled);
	m_elapsed = Time::Zero;
}

ProjectileComponent* ProjectileComponent::SetSpeed(Fixed speed)
{
	m_speed = speed;
	return this;
}

ProjectileComponent* ProjectileComponent::SetDirection(Vector2 direction)
{
	m_direction = direction.Normalised();
	m_pOwner->m_transform.SetOrientation(m_direction);
	return this;
}

ProjectileComponent* ProjectileComponent::SetTimeToLive(Time ttl)
{
	m_ttl = ttl;
	return this;
}

ProjectileComponent* ProjectileComponent::SetSelfDestruct(bool bSelfDestruct)
{
	m_bSelfDestruct = bSelfDestruct;
	return this;
}
} // namespace LittleEngine
