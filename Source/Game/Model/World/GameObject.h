#pragma once
#include "Core/CoreTypes.h"
#include "Engine/Input/LEInput.h"
#include "Model/Inheritable.h"
#include "Model/World/TokenHandler.h"

namespace LittleEngine
{
class GameObject : public Inheritable
{
protected:
	TokenHandler<Token> m_tokenHandler;

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
