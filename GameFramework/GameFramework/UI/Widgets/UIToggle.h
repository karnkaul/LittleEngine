#pragma once
#include "Delegate.hpp"
#include "LittleEngine/UI/UIWidget.h"
#include "LittleEngine/UI/UIStyle.h"

namespace LittleEngine
{
struct UIToggleData
{
	Vector2 boxSize = {40, 40};
	Colour onColour = Colour::Cyan;
	Colour offColour = Colour::White;
	String label;
	bool bSetOn;
};

using OnChanged = Core::Delegate<bool>;

class UIToggle : public UIWidget
{
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

	OnChanged::Token InitToggle(const UIToggleData& data, OnChanged::Callback onChanged);
	void SetText(const UIText& text);
	OnChanged::Token AddCallback(OnChanged::Callback callback);
	void SetOn(bool bOn);
	UIElement* GetRoot() const;

public:
	virtual void OnSelected() override;
	virtual void OnDeselected() override;
	virtual void OnInteractStart() override;
	virtual void OnInteractEnd() override;
};
} // namespace LittleEngine