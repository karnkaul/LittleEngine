#pragma once
#include "Delegate.hpp"
#include "LittleEngine/UI/UIWidget.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"

namespace LittleEngine
{
using OnClick = Core::Delegate<>;

class UIButton : public UIWidget
{
private:
	OnClick m_OnInteracted;
	UIElement* m_pRoot = nullptr;
	bool m_bSelected = false;

public:
	UIButton();
	UIButton(const String& name);
	virtual ~UIButton();

	void SetText(const UIText& uiText);
	OnClick::Token AddCallback(OnClick::Callback Callback);
	UIElement* GetButtonElement() const;

protected:
	virtual void OnInitWidget() override;
	virtual void OnSelected() override;
	virtual void OnDeselected() override;
	virtual void OnInteractStart() override;
	virtual void OnInteractEnd() override;
};
} // namespace LittleEngine
