#include "stdafx.h"
#include "Utils.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"
#include "LittleEngine/Game/Entity.h"
#include "LittleEngine/Game/GameManager.h"
#include "LittleEngine/Physics/Collider.h"
#include "LittleEngine/Physics/CollisionManager.h"
#include "LittleEngine/RenderLoop/RenderFactory.h"
#include "LittleEngine/Services/Services.h"
#include "CollisionComponent.h"

namespace LittleEngine
{
CollisionComponent::ColliderData::ColliderData(Collider* pCollider, Vector2 offset)
	: offset(offset), pCollider(pCollider)
{
}

#if DEBUGGING
CollisionComponent::ColliderData::ColliderData(Collider* pCollider, SFPrimitive* pSFPrimitive, Vector2 offset)
	: offset(offset), pCollider(pCollider), pSFPrimitive(pSFPrimitive)
{
}
#endif

CollisionComponent::~CollisionComponent()
{
	for (auto& data : m_pColliders)
	{
#if DEBUGGING
		if (data.pSFPrimitive)
		{
			data.pSFPrimitive->Destroy();
		}
#endif
		data.pCollider->m_bDestroyed = true;
	}
}


void CollisionComponent::OnCreated()
{
	SetName("Collision");
	m_signature = Maths::Random::Range(1, 10000);
}

void CollisionComponent::AddCircle(Fixed radius, Vector2 offset)
{
	CircleCollider* pCollider = Services::Game()->Physics()->CreateCircleCollider(m_pOwner->GetNameStr());
	pCollider->m_ignoreSig = m_signature;
	pCollider->m_name += ("_" + Strings::ToString(m_pColliders.size()));
	pCollider->SetCircle(radius);
#if DEBUGGING
	SFPrimitive* pPrimitive = Services::RFactory()->New(static_cast<LayerID>(LAYER_UI - 10));
	pPrimitive->SetSize({radius, radius}, SFShapeType::Circle)
		->SetPrimaryColour(Colour::Transparent)
		->SetSecondaryColour(Colour::Green)
		->SetOutline(Collider::s_debugShapeWidth)
		->SetEnabled(Collider::s_bShowDebugShape);
	m_pColliders.emplace_back(pCollider, pPrimitive, offset);
#else
	m_pColliders.emplace_back(pCollider, offset);
#endif
}

void CollisionComponent::AddAABB(const AABBData& aabbData, Vector2 offset)
{
	AABBCollider* pCollider = Services::Game()->Physics()->CreateAABBCollider(m_pOwner->GetNameStr());
	pCollider->m_ignoreSig = m_signature;
	pCollider->m_name += ("_" + Strings::ToString(m_pColliders.size()));
	pCollider->SetAABB(aabbData);
#if DEBUGGING
	SFPrimitive* pPrimitive = Services::RFactory()->New(static_cast<LayerID>(LAYER_UI - 10));
	pPrimitive->SetSize(2 * aabbData.upperBound, SFShapeType::Rectangle)
		->SetPrimaryColour(Colour::Transparent)
		->SetSecondaryColour(Colour::Green)
		->SetOutline(Collider::s_debugShapeWidth)
		->SetEnabled(Collider::s_bShowDebugShape);
	m_pColliders.emplace_back(pCollider, pPrimitive, offset);
#else
	m_pColliders.emplace_back(pCollider, offset);
#endif
}

TimingType CollisionComponent::GetComponentTiming() const
{
	return TimingType::LAST;
}

void CollisionComponent::Tick(Time /*dt*/)
{
	for (auto& data : m_pColliders)
	{
		Vector2 worldPosition = m_pOwner->m_transform.Position();
		data.pCollider->m_position = worldPosition;
#if DEBUGGING
		data.pSFPrimitive->SetPosition(worldPosition + data.offset);
		data.pSFPrimitive->SetEnabled(Collider::s_bShowDebugShape);
#endif
	}
}

void CollisionComponent::SetEnabled(bool bEnabled)
{
	AComponent::SetEnabled(bEnabled);

	for (auto& data : m_pColliders)
	{
		data.pCollider->m_bEnabled = bEnabled;
#if DEBUGGING
		data.pSFPrimitive->SetEnabled(bEnabled && Collider::s_bShowDebugShape);
#endif
	}
}
} // namespace LittleEngine
