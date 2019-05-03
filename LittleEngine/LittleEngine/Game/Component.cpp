#include "stdafx.h"
#include "Logger.h"
#include "Component.h"
#include "Entity.h"

namespace LittleEngine
{
AComponent::AComponent() : WorldObject("Untitled", "Component", true)
{
}

AComponent::~AComponent()
{
	LOG_D("%s detached from %s and destroyed", LogNameStr(), m_pOwner->LogNameStr());
}

TimingType AComponent::GetComponentTiming() const
{
	return TimingType::DEFAULT;
}

void AComponent::SetEnabled(bool bEnabled)
{
	Assert(m_pOwner, "Component Owner is null!");
	m_bEnabled = bEnabled;
}

void AComponent::Tick(Time /*dt*/)
{
	Assert(m_pOwner, "Component Owner is null!");
}

void AComponent::RegenerateLogNameStr()
{
	String suffix = m_typeName.empty() ? "" : "/" + m_typeName;
	suffix += "=>" + m_pOwner->m_name;
	m_logName = "[" + m_name + suffix + "]";
}

void AComponent::Destruct()
{
	m_bDestroyed = true;
}

void AComponent::SetOwner(Entity& owner)
{
	m_pOwner = &owner;
	m_pOwner->m_pComponents.push_back(this);
	OnCreated();
}
} // namespace LittleEngine
