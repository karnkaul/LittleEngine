#pragma once
#include "Core/CoreTypes.h"
#include "Core/TokenHandler.h"
#include "Inheritable.h"
#include "LittleEngine/Input/EngineInput.h"

namespace LittleEngine
{
class WorldObject : public Inheritable
{
private:
	Core::TokenHandler<EngineInput::Token> m_tokenHandler;

public:
	WorldObject(bool bSilent = false);
	WorldObject(String name, String baseClass = "", bool bSilent = false);
	~WorldObject() override;

protected:
	void BindInput(EngineInput::Delegate callback);
};
} // namespace LittleEngine
