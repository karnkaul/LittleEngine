#include "Core/Logger.h"
#include "Component.h"
#include "Entity.h"
#include "../GameManager.h"

namespace LittleEngine
{
AComponent::AComponent() : GameObject("Untitled", "Component", true)
{
	Assert(g_pGameManager, "Game Manager is null!");
}

AComponent::~AComponent()
{
	LOG_D("%s destroyed", m_logName.c_str());
}

TimingType AComponent::Timing() const
{
	return TimingType::Default;
}

void AComponent::SetEnabled(bool bEnabled)
{
	Assert(m_pOwner, "Component Owner is null!");
	m_bEnabled = bEnabled;
}

void AComponent::OnRespawn() {}

void AComponent::RegenerateLogNameStr()
{
	m_logName = "[";
	m_logName += m_name;
	if (!m_typeName.empty())
	{
		m_logName += "/";
		m_logName += m_typeName;
	}
	m_logName += "=>";
	m_logName += m_pOwner->m_name;
	m_logName += "]";
}

void AComponent::Destruct()
{
	m_bDestroyed = true;
}

void AComponent::OnCreate(Entity& owner)
{
	m_pOwner = &owner;
	OnCreated();
}
} // namespace LittleEngine
