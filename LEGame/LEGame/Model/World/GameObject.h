#pragma once
#include "Core/CoreTypes.h"
#include "LittleEngine/Input/LEInput.h"
#include "LEGame/Model/Inheritable.h"
#include "LEGame/Model/World/InputTokenHandler.h"

namespace LittleEngine
{
class GameObject : public Inheritable
{
protected:
	InputTokenHandler<LEInput::Token> m_tokenHandler;

public:
	GameObject(bool bSilent = false);
	GameObject(String name, String baseClass = "", bool bSilent = false);
	~GameObject() override;

protected:
	void BindInput(LEInput::Delegate callback, bool bForceEveryFrame = false);
	class LERenderer* Renderer() const;

private:
	friend class GameManager;
};
} // namespace LittleEngine
