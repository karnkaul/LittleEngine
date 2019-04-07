#include "stdafx.h"
#include "WorldObject.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/Game/GameManager.h"

namespace LittleEngine
{
WorldObject::WorldObject(String name, String baseClass) : Inheritable(std::move(name), std::move(baseClass))
{
}

WorldObject::~WorldObject() = default;

void WorldObject::BindInput(const EngineInput::Delegate& Callback)
{
	m_tokenHandler.AddToken(Services::Game()->Input()->Register(Callback));
}
} // namespace LittleEngine
