#pragma once
#include "Delegate.hpp"
#include "LittleEngine/UI/UIWidget.h"
#include "LittleEngine/UI/UIText.h"

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
	UIToggle* SetOn(bool bOn);
	UIToggle* SetText(UIText text);
	UIToggle* SetOnColour(Colour onColour);
	UIToggle* SetOffColour(Colour offColour);
	UIToggle* SetBoxSize(Vector2 size);
	OnChanged::Token AddCallback(OnChanged::Callback callback);

	UIElement* GetRoot() const;

public:
	void SetInteractable(bool bInteractable) override;

protected:
	void OnCreated() override;
	void OnSelected() override;
	void OnDeselected() override;
	void OnInteractStart() override;
	void OnInteractEnd(bool bInteract) override;
	void Tick(Time dt) override;
};
} // namespace LittleEngine