#pragma once
#include "Core/CoreTypes.h"
#include "Core/TokenHandler.h"
#include "Inheritable.h"
#include "LittleEngine/Input/LEInput.h"

namespace LittleEngine
{
class GameObject : public Inheritable
{
private:
	Core::TokenHandler<LEInput::Token> m_tokenHandler;
	class GameManager* m_pManager = nullptr;

public:
	GameObject(bool bSilent = false);
	GameObject(String name, String baseClass = "", bool bSilent = false);
	~GameObject() override;

protected:
	void BindInput(LEInput::Delegate callback);
	class LERenderer* Renderer() const;

private:
	void SetManager(GameManager& manager);

	friend class GameManager;
};
} // namespace LittleEngine
