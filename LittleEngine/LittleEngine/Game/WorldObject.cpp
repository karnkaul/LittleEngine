#include "stdafx.h"
#include "WorldObject.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/Game/GameManager.h"

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

void WorldObject::BindInput(EngineInput::Delegate callback)
{
	m_tokenHandler.AddToken(Services::Game()->Input()->Register(std::move(callback)));
}
} // namespace LittleEngine
