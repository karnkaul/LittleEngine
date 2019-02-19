#pragma once
#include "LittleEngine/UI/UIWidget.h"
#include "Delegate.hpp"

namespace LittleEngine
{
struct UIToggleData
{
	UIText labelText;
	Vector2 totalSize = Vector2(250, 100);
	Vector2 boxSize = Vector2(40, 40);
	Colour bgColour = Colour::Transparent;
	Colour onColour = Colour::Green;
	Colour offColour = Colour::White;
	Colour interactColour = Colour::Yellow;
	Colour borderColourS = Colour::Red;
	Colour borderColourNS = Colour::Transparent;
	Fixed borderSize = Fixed::Two;
	u32 textSize = 12;
	bool bSetOn = false;
};

using OnChanged = Core::Delegate<bool>;

class UIToggle : public UIWidget
{
private:
	UIElement* m_pRoot;
	UIElement* m_pLabel;
	UIElement* m_pToggle;
	OnChanged m_delegate;
	UIToggleData m_data;
	bool m_bOn = false;

public:
	UIToggle();
	UIToggle(const String& name);
	~UIToggle();

	OnChanged::Token InitToggle(const UIToggleData& data, OnChanged::Callback onChanged);
	UIElement* GetRoot() const;

public:
	virtual void OnSelected() override;
	virtual void OnDeselected() override;
	virtual void OnInteractStart() override;
	virtual void OnInteractEnd() override;
};
}