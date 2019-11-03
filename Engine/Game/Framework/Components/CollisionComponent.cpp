#include "SFMLAPI/Rendering/Primitives/SFCircle.h"
#include "SFMLAPI/Rendering/Primitives/SFRect.h"
#include "Engine/Physics/Collider.h"
#include "Engine/Physics/LEPhysics.h"
#include "Engine/Rendering/LERenderer.h"
#include "Game/Model/World/Entity.h"
#include "Game/Model/GameManager.h"
#include "CollisionComponent.h"

namespace LittleEngine
{
CollisionComponent::CollisionComponent() = default;

CollisionComponent::~CollisionComponent()
{
	for (auto& data : m_pColliders)
	{
#if defined(DEBUGGING)
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

void CollisionComponent::Tick(Time /*dt*/)
{
	for (auto& data : m_pColliders)
	{
		Vector2 worldPosition = m_pOwner->WorldMatrix().Position();
		data.pCollider->m_position = worldPosition;
#if defined(DEBUGGING)
		data.pShape->SetPosition(worldPosition + data.offset);
		data.pShape->SetEnabled(Collider::s_bShowDebugShape);
#endif
	}
}

TimingType CollisionComponent::Timing() const
{
	return TimingType::Last;
}

void CollisionComponent::SetEnabled(bool bEnabled)
{
	Super::SetEnabled(bEnabled);

	for (auto& data : m_pColliders)
	{
		data.pCollider->m_bEnabled = bEnabled;
#if defined(DEBUGGING)
		data.pShape->SetEnabled(bEnabled && Collider::s_bShowDebugShape);
#endif
	}
}

void CollisionComponent::AddCircle(Fixed diameter, Vector2 offset)
{
	CircleCollider* pCollider = g_pGameManager->Physics()->CreateCircleCollider(std::string(m_pOwner->Name()));
	pCollider->m_ignoreSig = m_signature;
	pCollider->m_name += ("_" + Strings::ToString(m_pColliders.size()));
	pCollider->SetCircle(diameter);
#if defined(DEBUGGING)
	auto pCircle = g_pGameManager->Renderer()->New<SFCircle>(LayerID::DebugWorld);
	pCircle->SetDiameter(diameter)
		->SetOutline(Collider::s_debugShapeWidth)
		->SetSecondaryColour(Colour::Green)
		->SetColour(Colour::Transparent)
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
	AABBCollider* pCollider = g_pGameManager->Physics()->CreateAABBCollider(std::string(m_pOwner->Name()));
	pCollider->m_ignoreSig = m_signature;
	pCollider->m_name += ("_" + Strings::ToString(m_pColliders.size()));
	pCollider->SetAABB(aabbData);
#if defined(DEBUGGING)
	auto pRect = g_pGameManager->Renderer()->New<SFRect>(LayerID::DebugWorld);
	pRect->SetSize(2 * aabbData.upperBound)
		->SetOutline(Collider::s_debugShapeWidth)
		->SetSecondaryColour(Colour::Green)
		->SetColour(Colour::Transparent)
		->SetEnabled(Collider::s_bShowDebugShape);
	ColliderData data{offset, pCollider, pRect};
	m_pColliders.emplace_back(std::move(data));
#else
	ColliderData data{offset, pCollider};
	m_pColliders.emplace_back(std::move(data));
#endif
}
} // namespace LittleEngine
