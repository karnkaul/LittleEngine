#pragma once
#include "Core/CoreTypes.h"
#include "Core/TokenHandler.h"
#include "LEGame/GameModel/Inheritable.h"
#include "LittleEngine/Input/LEInput.h"

namespace LittleEngine
{
class WorldObject : public Inheritable
{
private:
	Core::TokenHandler<LEInput::Token> m_tokenHandler;

public:
	WorldObject(bool bSilent = false);
	WorldObject(String name, String baseClass = "", bool bSilent = false);
	~WorldObject() override;

protected:
	void BindInput(LEInput::Delegate callback);
};
} // namespace LittleEngine
