#pragma once
#include "Delegate.hpp"
#include "LittleEngine/UI/UIWidget.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"

namespace LittleEngine
{
using Delegate = Core::Delegate<>;

struct UIButtonData
{
	Vector2 size;
	u32 textSize = 20;
	Fixed notSelectedBorder = Fixed::Zero;
	Fixed selectedBorder = Fixed::Zero;
	Colour notSelectedFill = Colour::Transparent;
	Colour notSelectedOutline = Colour::Transparent;
	Colour selectedFill = Colour::Transparent;
	Colour selectedOutline = Colour::Transparent;
	Colour interactingFill = Colour::Transparent;

	static const UIButtonData DebugButton();
};

class UIButton : public UIWidget
{
private:
	Delegate m_OnInteracted;
	UIButtonData m_data;
	UIElement* m_pUIElement = nullptr;
	bool m_bSelected = false;

public:
	UIButton();
	UIButton(const String& name);
	virtual ~UIButton();

	void InitButton(UIButtonData&& data);
	void SetText(const UIText& uiText);
	Delegate::Token AddCallback(Delegate::Callback Callback);
	UIElement* GetButtonElement() const;

protected:
	virtual void OnSelected() override;
	virtual void OnDeselected() override;
	virtual void OnInteractStart() override;
	virtual void OnInteractEnd() override;
};
} // namespace LittleEngine
