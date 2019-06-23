#include "stdafx.h"
#include "Core/Asserts.h"
#include "Core/Logger.h"
#include "SFMLAPI/Rendering/Primitives/SFRect.h"
#include "LittleEngine/Renderer/LERenderer.h"
#include "LittleEngine/Debug/Tweakable.h"
#include "Component.h"
#include "Entity.h"
#include "Camera.h"
#include "../GameManager.h"

namespace LittleEngine
{
#if DEBUGGING
bool Entity::s_bShowOrientation = false;
Vector2 Entity::s_orientationWidthHeight = {100, 2};
Array<Colour, 2> Entity::s_xyColours = {Colour::Red, Colour::Green};
TweakBool(orientationVectors, &Entity::s_bShowOrientation);
#endif

Entity::Entity()
{
	Assert(g_pGameManager, "Game Manager is null!");
	Camera* pWorldCam = g_pGameManager->WorldCamera();
	if (pWorldCam)
	{
		m_transform.SetParent(pWorldCam->m_transform);
	}
}

Entity::~Entity()
{
	m_pComponents.clear();
#if DEBUGGING
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

void Entity::OnCreate(String name)
{
	SetNameAndType(std::move(name), "Entity");
#if DEBUGGING
	LayerID layer = static_cast<LayerID>(LAYER_DEBUG_UI);
	m_pO_x = g_pGameManager->Renderer()->New<SFRect>(layer);
	m_pO_x->SetSize({100, 2})->SetPivot({-1, 0})->SetPrimaryColour(s_xyColours[0]);
	m_pO_y = g_pGameManager->Renderer()->New<SFRect>(layer);
	m_pO_y->SetSize({2, 100})->SetPivot({0, -1})->SetPrimaryColour(s_xyColours[1]);
#endif
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
#if DEBUGGING
	m_pO_x->SetEnabled(s_bShowOrientation);
	m_pO_y->SetEnabled(s_bShowOrientation);
	if (s_bShowOrientation)
	{
		m_pO_x->SetPosition(m_transform.GetWorldPosition(), true)->SetOrientation(m_transform.GetWorldOrientation(), true);
		m_pO_y->SetPosition(m_transform.GetWorldPosition(), true)->SetOrientation(m_transform.GetWorldOrientation(), true);
	}
#endif
	Core::RemoveIf<AComponent*>(m_pComponents,
								[](AComponent* pComponent) { return pComponent->m_bDestroyed; });
}

void Entity::OnCreated()
{
}
} // namespace LittleEngine
