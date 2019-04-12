#pragma once
#include "CoreTypes.h"
#include "TokenHandler.hpp"
#include "Inheritable.h"
#include "LittleEngine/Input/EngineInput.h"

namespace LittleEngine
{
class WorldObject : public Inheritable
{
private:
	Core::TokenHandler<EngineInput::Token> m_tokenHandler;

public:
	WorldObject(String name, String baseClass = "", bool bSilent = false);
	~WorldObject() override;

protected:
	void BindInput(EngineInput::Delegate callback);
};
} // namespace LittleEngine
