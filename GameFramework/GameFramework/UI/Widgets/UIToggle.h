#pragma once
#include "Delegate.hpp"
#include "LittleEngine/UI/UIWidget.h"
#include "LittleEngine/UI/UIStyle.h"

namespace LittleEngine
{
class UIToggle : public UIWidget
{
public:
	using OnChanged = Core::Delegate<bool>;

private:
	struct UIToggleData
	{
		Vector2 boxSize = {40, 40};
		Colour onColour = Colour::Cyan;
		Colour offColour = Colour::White;
	};

private:
	UIToggleData m_data;
	OnChanged m_delegate;
	UIElement* m_pRoot;
	UIElement* m_pLabel;
	UIElement* m_pToggle;
	bool m_bOn = false;

public:
	UIToggle();
	UIToggle(const String& name);
	~UIToggle();

	UIToggle* SetOn(bool bOn);
	UIToggle* SetText(const UIText& text);
	UIToggle* SetOnColour(Colour onColour);
	UIToggle* SetOffColour(Colour offColour);
	UIToggle* SetBoxSize(const Vector2& size);
	OnChanged::Token AddCallback(const OnChanged::Callback& callback);

	UIElement* GetRoot() const;

public:
	virtual void SetInteractable(bool bInteractable) override;

protected:
	virtual void OnInitWidget() override;
	virtual void OnSelected() override;
	virtual void OnDeselected() override;
	virtual void OnInteractStart() override;
	virtual void OnInteractEnd(bool bInteract) override;
	virtual void Tick(Time dt) override;
};
} // namespace LittleEngine