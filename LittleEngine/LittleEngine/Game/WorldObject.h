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
	WorldObject(String name, String baseClass = "");
	~WorldObject() override;

protected:
	void BindInput(const EngineInput::Delegate& Callback);
};
} // namespace LittleEngine
