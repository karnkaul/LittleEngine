#include "stdafx.h"
#include "Entity.h"

namespace LittleEngine
{
Entity::Entity(const String& name) : WorldObject(name, "Entity")
{
}

Entity::~Entity()
{
	m_pComponents.clear();
	LogD(LogName() + " destroyed");
}

void Entity::SetEnabled(bool bEnabled)
{
	m_bEnabled = bEnabled;
	for (auto pComponent : m_pComponents)
	{
		pComponent->SetEnabled(bEnabled);
	}
	String action = m_bEnabled ? " Enabled" : " Disabled";
	LogD(LogName() + action);
}

void Entity::Destruct()
{
	for (auto pComponent : m_pComponents)
	{
		pComponent->Destruct();
	}
	m_bDestroyed = true;
}

void Entity::Tick(Time)
{
	Core::CleanVector<Component*>(m_pComponents,
								  [](Component* pComponent) { return pComponent->m_bDestroyed; });
}
} // namespace LittleEngine
