#include "stdafx.h"
#include "Logger.h"
#include "Component.h"
#include "Entity.h"

namespace LittleEngine
{
Component::Component() : WorldObject("Untitled", "Component", true)
{
}

Component::~Component()
{
	LOG_D("%s detached from %s and destroyed", LogNameStr(), m_pOwner->LogNameStr());
}

TimingType Component::GetComponentTiming() const
{
	return TimingType::DEFAULT;
}

void Component::OnCreated()
{
}

void Component::SetEnabled(bool bEnabled)
{
	Assert(m_pOwner, "Component Owner is null!");
	m_bEnabled = bEnabled;
}

void Component::Tick(Time /*dt*/)
{
	Assert(m_pOwner, "Component Owner is null!");
}

void Component::Destruct()
{
	m_bDestroyed = true;
}

void Component::SetOwner(Entity& owner)
{
	m_pOwner = &owner;
	m_pOwner->m_pComponents.push_back(this);
	OnCreated();
}
} // namespace LittleEngine
