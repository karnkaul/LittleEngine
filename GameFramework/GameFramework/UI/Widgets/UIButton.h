#pragma once
#include "Delegate.hpp"
#include "LittleEngine/UI/UIWidget.h"
#include "LittleEngine/UI/UIText.h"

namespace LittleEngine
{
class UIButton : public UIWidget
{
public:
	using OnClick = Core::Delegate<>;

private:
	OnClick m_OnInteracted;
	UIElement* m_pRoot = nullptr;

public:
	UIButton();
	UIButton(String name);
	~UIButton() override;

	void SetText(UIText uiText);
	OnClick::Token AddCallback(OnClick::Callback callback);
	UIElement* GetButtonElement() const;

public:
	void SetInteractable(bool bInteractable) override;

protected:
	void OnInitWidget() override;
	void OnSelected() override;
	void OnDeselected() override;
	void OnInteractStart() override;
	void OnInteractEnd(bool bInteract) override;
};
} // namespace LittleEngine
