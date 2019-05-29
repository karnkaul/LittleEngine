#include "stdafx.h"
#include "WorldObject.h"
#include "LEGame/GameModel/GameManager.h"

namespace LittleEngine
{
WorldObject::WorldObject(bool bSilent) : Inheritable(bSilent)
{
}

WorldObject::WorldObject(String name, String baseClass, bool bSilent)
	: Inheritable(std::move(name), std::move(baseClass), bSilent)
{
}

WorldObject::~WorldObject() = default;

void WorldObject::BindInput(LEInput::Delegate callback)
{
	Assert(g_pGameManager, "Game Manager is null!");
	m_tokenHandler.AddToken(g_pGameManager->Input()->Register(std::move(callback)));
}
} // namespace LittleEngine
