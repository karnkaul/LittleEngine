#pragma once
#include "Game/Model/UI/UIWidget.h"

namespace LittleEngine
{
class UIToggle : public UIWidget
{
public:
	using OnChanged = LE::Delegate<bool>;

private:
	struct UIToggleData
	{
		Vector2 boxSize = {40, 40};
		Colour onColour = Colour::Cyan;
		Colour offColour = Colour(150, 150, 150, 255);
	};

private:
	UIToggleData m_data;
	OnChanged m_delegate;
	UIElement* m_pLabel;
	UIElement* m_pToggle;
	bool m_bOn = false;

public:
	UIToggle* SetOn(bool bOn);
	UIToggle* SetText(UIText text);
	UIToggle* SetOnColour(Colour onColour);
	UIToggle* SetOffColour(Colour offColour);
	UIToggle* SetBackground(Colour colour);
	UIToggle* SetBoxSize(Vector2 size);
	Token AddCallback(OnChanged::Callback callback);

protected:
	void OnCreated() override;
	void OnSelected() override;
	void OnDeselected() override;
	void OnInteractStart() override;
	void OnInteractEnd(bool bInteract) override;
	void OnSetInteractable(bool bInteractable) override;
};
} // namespace LittleEngine
