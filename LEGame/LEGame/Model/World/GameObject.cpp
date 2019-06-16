#include "stdafx.h"
#include "GameObject.h"
#include "../GameManager.h"

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

void GameObject::BindInput(LEInput::Delegate callback, bool bForceEveryFrame)
{
	Assert(g_pGameManager, "Manager is null!");
	m_tokenHandler.AddToken(g_pGameManager->Input()->Register(std::move(callback), bForceEveryFrame));
}

LERenderer* GameObject::Renderer() const
{
	Assert(g_pGameManager, "Manager is null!");
	return g_pGameManager->Renderer();
}
} // namespace LittleEngine
