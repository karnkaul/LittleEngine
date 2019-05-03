#include "stdafx.h"
#include "Logger.h"
#include "Camera.h"
#include "Component.h"
#include "Entity.h"
#include "Camera.h"
#include "LittleEngine/Services/Services.h"

namespace LittleEngine
{
Entity::Entity()
{
	Camera* pWorldCam = Services::WorldCamera();
	if (pWorldCam)
	{
		m_transform.SetParent(pWorldCam->m_transform);
	}
}

Entity::~Entity()
{
	m_pComponents.clear();
}

void Entity::OnCreate(String name)
{
	SetNameAndType(std::move(name), "Entity");
	OnCreated();
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

void Entity::Tick(Time /*dt*/)
{
	Core::CleanVector<AComponent*>(m_pComponents,
								  [](AComponent* pComponent) { return pComponent->m_bDestroyed; });
}

void Entity::OnCreated()
{
}

} // namespace LittleEngine
