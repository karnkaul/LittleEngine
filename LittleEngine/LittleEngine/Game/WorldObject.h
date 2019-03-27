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
	WorldObject(const String& name, const String& baseClass = "");
	virtual ~WorldObject();

protected:
	void BindInput(const EngineInput::Delegate& Callback);
};
} // namespace LittleEngine
