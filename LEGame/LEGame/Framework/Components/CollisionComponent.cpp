#include "stdafx.h"
#include "Core/Utils.h"
#include "SFMLAPI/Rendering/Primitives/SFCircle.h"
#include "SFMLAPI/Rendering/Primitives/SFRect.h"
#include "LittleEngine/Physics/Collider.h"
#include "LittleEngine/Physics/CollisionManager.h"
#include "LittleEngine/Renderer/LERenderer.h"
#include "LEGame/Model/World/Entity.h"
#include "LEGame/Model/GameManager.h"
#include "CollisionComponent.h"

namespace LittleEngine
{
CollisionComponent::~CollisionComponent()
{
	for (auto& data : m_pColliders)
	{
#if DEBUGGING
		if (data.pShape)
		{
			data.pShape->Destroy();
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

void CollisionComponent::AddCircle(Fixed diameter, Vector2 offset)
{
	CircleCollider* pCollider = g_pGameManager->Physics()->CreateCircleCollider(m_pOwner->GetNameStr());
	pCollider->m_ignoreSig = m_signature;
	pCollider->m_name += ("_" + Strings::ToString(m_pColliders.size()));
	pCollider->SetCircle(diameter);
#if DEBUGGING
	SFCircle* pCircle = g_pGameManager->Renderer()->New<SFCircle>(static_cast<LayerID>(LAYER_UI - 10));
	pCircle->SetDiameter(diameter)
		->SetOutline(Collider::s_debugShapeWidth)
		->SetSecondaryColour(Colour::Green)
		->SetPrimaryColour(Colour::Transparent)
		->SetEnabled(Collider::s_bShowDebugShape);
	ColliderData data{offset, pCollider, pCircle};
	m_pColliders.emplace_back(std::move(data));
#else
	ColliderData data{offset, pCollider};
	m_pColliders.emplace_back(std::move(data));
#endif
}

void CollisionComponent::AddAABB(const AABBData& aabbData, Vector2 offset)
{
	AABBCollider* pCollider = g_pGameManager->Physics()->CreateAABBCollider(m_pOwner->GetNameStr());
	pCollider->m_ignoreSig = m_signature;
	pCollider->m_name += ("_" + Strings::ToString(m_pColliders.size()));
	pCollider->SetAABB(aabbData);
#if DEBUGGING
	SFRect* pRect = g_pGameManager->Renderer()->New<SFRect>(static_cast<LayerID>(LAYER_UI - 10));
	pRect->SetSize(2 * aabbData.upperBound)
		->SetOutline(Collider::s_debugShapeWidth)
		->SetSecondaryColour(Colour::Green)
		->SetPrimaryColour(Colour::Transparent)
		->SetEnabled(Collider::s_bShowDebugShape);
	ColliderData data{offset, pCollider, pRect};
	m_pColliders.emplace_back(std::move(data));
#else
	ColliderData data{offset, pCollider};
	m_pColliders.emplace_back(std::move(data));
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
		data.pShape->SetPosition(worldPosition + data.offset);
		data.pShape->SetEnabled(Collider::s_bShowDebugShape);
#endif
	}
}

void CollisionComponent::SetEnabled(bool bEnabled)
{
	Super::SetEnabled(bEnabled);

	for (auto& data : m_pColliders)
	{
		data.pCollider->m_bEnabled = bEnabled;
#if DEBUGGING
		data.pShape->SetEnabled(bEnabled && Collider::s_bShowDebugShape);
#endif
	}
}
} // namespace LittleEngine
