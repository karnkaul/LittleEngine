#include "stdafx.h"
#include "WorldObject.h"
#include "LittleEngine/Services/Services.h"

namespace LittleEngine
{
WorldObject::WorldObject(const String& name, const String& baseClass) : Inheritable(name, baseClass)
{
}

WorldObject::~WorldObject() = default;

void WorldObject::BindInput(EngineInput::Delegate Callback)
{
	m_tokenHandler.AddToken(Services::Game()->Input()->Register(Callback));
}
} // namespace LittleEngine
