#pragma once
#include "Delegate.hpp"
#include "LittleEngine/UI/UIWidget.h"

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

	void SetText(const struct UIText& uiText);
	OnClick::Token AddCallback(const OnClick::Callback& Callback);
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
