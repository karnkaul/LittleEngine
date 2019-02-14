#include "stdafx.h"
#include "RenderComponent.h"
#include "LittleEngine/Game/Entity.h"
#include "LittleEngine/Services/Services.h"

namespace LittleEngine
{
RenderComponent::~RenderComponent()
{
	Services::RHeap()->Destroy(m_pSFPrimitive);
}

TimingType RenderComponent::GetComponentTiming() const
{
	return TimingType::POST;
}

void RenderComponent::OnCreated()
{
	m_name = "Render";
	m_pSFPrimitive = Services::RHeap()->New();
	m_pSFPrimitive->SetEnabled(true);
}

void RenderComponent::SetEnabled(bool bEnabled)
{
	Component::SetEnabled(bEnabled);

	m_pSFPrimitive->SetEnabled(bEnabled);
}

void RenderComponent::Tick(Time)
{
	UpdatePrimitive();
}

void RenderComponent::UpdatePrimitive()
{
	m_pSFPrimitive->SetScale(m_pOwner->m_transform.Scale(), m_pOwner->m_bReset);
	m_pSFPrimitive->SetOrientation(m_pOwner->m_transform.Orientation(), m_pOwner->m_bReset);
	m_pSFPrimitive->SetPosition(m_pOwner->m_transform.Position(), m_pOwner->m_bReset);
	m_pOwner->m_bReset = false;
}
} // namespace LittleEngine
