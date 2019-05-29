#include "stdafx.h"
#include "GameObject.h"
#include "GameManager.h"

namespace LittleEngine
{
GameObject::GameObject(bool bSilent) : Inheritable(bSilent)
{
}

GameObject::GameObject(String name, String baseClass, bool bSilent)
	: Inheritable(std::move(name), std::move(baseClass), bSilent)
{
}

GameObject::~GameObject() = default;

void GameObject::BindInput(LEInput::Delegate callback)
{
	Assert(m_pManager, "Manager is null!");
	m_tokenHandler.AddToken(m_pManager->Input()->Register(std::move(callback)));
}

LERenderer* GameObject::Renderer() const
{
	Assert(m_pManager, "Manager is null!");
	return m_pManager->Renderer();
}

void GameObject::SetManager(GameManager& manager)
{
	m_pManager = &manager;
}
} // namespace LittleEngine
