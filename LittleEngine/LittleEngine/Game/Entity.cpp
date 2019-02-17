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
	LOG_D("%s destroyed", LogNameStr());
}

void Entity::SetEnabled(bool bEnabled)
{
	m_bEnabled = bEnabled;
	for (auto pComponent : m_pComponents)
	{
		pComponent->SetEnabled(bEnabled);
	}
	LOG_D("%s %s", LogNameStr(), m_bEnabled ? " Enabled" : " Disabled");
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