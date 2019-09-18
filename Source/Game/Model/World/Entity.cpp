#include "Core/Asserts.h"
#include "Core/Logger.h"
#include "SFMLAPI/Rendering/Primitives/SFRect.h"
#include "Engine/Renderer/LERenderer.h"
#include "Engine/Debug/Tweakable.h"
#include "Component.h"
#include "Entity.h"
#include "Camera.h"
#include "../GameManager.h"

namespace LittleEngine
{
#if defined(DEBUGGING)
bool Entity::s_bShowOrientation = false;
Vector2 Entity::s_orientationWidthHeight = {100, 2};
Array<Colour, 2> Entity::s_xyColours = {Colour(200, 0, 0), Colour(0, 200, 0)};
TweakBool(orientationVectors, &Entity::s_bShowOrientation);
#endif

Entity::Entity() = default;

Entity::~Entity()
{
	m_components.clear();
#if defined(DEBUGGING)
	if (m_pO_x)
	{
		m_pO_x->Destroy();
	}
	if (m_pO_y)
	{
		m_pO_y->Destroy();
	}
#endif
}

void Entity::SetEnabled(bool bEnabled)
{
	m_bEnabled = bEnabled;
	for (auto pComponent : m_components)
	{
		pComponent->SetEnabled(bEnabled);
	}
	LOG_D("%s %s", LogNameStr(), m_bEnabled ? "enabled" : "disabled");
}

void Entity::Tick(Time /*dt*/)
{
	if (!m_bEnabled || IsDestroyed())
	{
		return;
	}
#if defined(DEBUGGING)
	m_pO_x->SetEnabled(s_bShowOrientation);
	m_pO_y->SetEnabled(s_bShowOrientation);
	if (s_bShowOrientation)
	{
		Matrix3 mat = WorldMatrix();
		m_pO_x->SetPosition(mat.Position(), true)->SetOrientation(mat.Orientation(), true);
		m_pO_y->SetPosition(mat.Position(), true)->SetOrientation(mat.Orientation(), true);
	}
#endif
	Core::RemoveIf<AComponent*>(m_components, [](AComponent* pComponent) { return pComponent->m_bDestroyed; });
}

void Entity::OnCreate(String name)
{
	SetNameAndType(std::move(name), "Entity");
#if defined(DEBUGGING)
	LayerID layer = LayerID::Debug_UI;
	m_pO_x = g_pGameManager->Renderer()->New<SFRect>(layer);
	m_pO_x->SetSize({100, 2})->SetPivot({-1, 0})->SetPrimaryColour(s_xyColours[0]);
	m_pO_y = g_pGameManager->Renderer()->New<SFRect>(layer);
	m_pO_y->SetSize({2, 100})->SetPivot({0, -1})->SetPrimaryColour(s_xyColours[1]);
#endif
	OnCreated();
	m_state = State::Spawned;
}

void Entity::Destruct()
{
	if (m_pPool)
	{
		Despawn();
		return;
	}

	for (auto pComponent : m_components)
	{
		pComponent->Destruct();
	}
	m_state = State::Destroyed;
}

void Entity::Respawn()
{
	SetEnabled(true);
	OnRespawn();
	m_state = State::Spawned;
}

void Entity::Despawn()
{
	SetEnabled(false);
	OnDespawn();
	m_state = State::Despawned;
}

Entity::State Entity::GetState() const
{
	return m_state;
}

bool Entity::IsDestroyed() const
{
	return m_state == State::Destroyed;
}

Matrix3 Entity::WorldMatrix() const
{
	const Matrix3& view = g_pGameManager->WorldCamera()->m_transform.WorldMatrix();
	const Matrix3& model = m_transform.WorldMatrix();
	return model * view;
}

void Entity::OnCreated() {}

void Entity::OnRespawn() {}

void Entity::OnDespawn() {}
} // namespace LittleEngine
