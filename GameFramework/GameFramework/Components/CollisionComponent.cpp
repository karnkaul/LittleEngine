#include "stdafx.h"
#include "CollisionComponent.h"
#include "Utils.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/RenderLoop/RenderHeap.h"
#include "LittleEngine/Game/GameManager.h"

namespace LittleEngine
{
CollisionComponent::ColliderData::ColliderData(Collider* pCollider, const Vector2& offset)
	: offset(offset), pCollider(pCollider)
{
}

#if DEBUGGING
CollisionComponent::ColliderData::ColliderData(Collider* pCollider, SFPrimitive* pSFPrimitive, const Vector2& offset)
	: offset(offset), pCollider(pCollider), pSFPrimitive(pSFPrimitive)
{
}
#endif

CollisionComponent::CollisionComponent()
{
	SetName("Collision");
	m_signature = Maths::Random::Range(1, 10000);
}

CollisionComponent::~CollisionComponent()
{
	for (auto& data : m_pColliders)
	{
#if DEBUGGING
		Services::RHeap()->Destroy(data.pSFPrimitive);
#endif
		data.pCollider->m_bDestroyed = true;
	}
}

void CollisionComponent::AddCircle(const Fixed& radius, const Vector2& offset)
{
	CircleCollider* pCollider = Services::Game()->Physics()->CreateCircleCollider(m_pOwner->GetNameStr());
	pCollider->m_ignoreSig = m_signature;
	pCollider->m_name += ("_" + Strings::ToString(m_pColliders.size()));
	pCollider->SetCircle(radius);
#if DEBUGGING
	SFPrimitive* pPrimitive = Services::RHeap()->New();
	pPrimitive->SetSize({radius, radius}, SFShapeType::Circle)
		->SetPrimaryColour(Colour::Transparent)
		->SetSecondaryColour(Colour::Green)
		->SetOutline(Collider::s_debugShapeWidth)
		->SetLayer(LAYER_TOP)
		->SetEnabled(Collider::s_bShowDebugShape);
	m_pColliders.emplace_back(pCollider, pPrimitive, offset);
#else
	m_pColliders.emplace_back(pCollider, offset);
#endif
}

void CollisionComponent::AddAABB(const AABBData& aabbData, const Vector2& offset)
{
	AABBCollider* pCollider = Services::Game()->Physics()->CreateAABBCollider(m_pOwner->GetNameStr());
	pCollider->m_ignoreSig = m_signature;
	pCollider->m_name += ("_" + Strings::ToString(m_pColliders.size()));
	pCollider->SetAABB(aabbData);
#if DEBUGGING
	SFPrimitive* pPrimitive = Services::RHeap()->New();
	pPrimitive->SetSize(2 * aabbData.upperBound, SFShapeType::Rectangle)
		->SetPrimaryColour(Colour::Transparent)
		->SetSecondaryColour(Colour::Green)
		->SetOutline(Collider::s_debugShapeWidth)
		->SetLayer(LAYER_TOP)
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

void CollisionComponent::Tick(Time)
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
	Component::SetEnabled(bEnabled);

	for (auto& data : m_pColliders)
	{
		data.pCollider->m_bEnabled = bEnabled;
#if DEBUGGING
		data.pSFPrimitive->SetEnabled(bEnabled && Collider::s_bShowDebugShape);
#endif
	}
}
} // namespace LittleEngine
