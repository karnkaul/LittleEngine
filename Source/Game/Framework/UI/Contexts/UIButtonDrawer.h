#pragma once
#include "Core/CoreTypes.h"
#include "Core/TRange.h"
#include "Model/UI/UIContext.h"
#include "Model/UI/UIStyle.h"
#include "Framework/UI/Widgets/UIButton.h"

namespace LittleEngine
{
class UIButtonDrawer : public UIContext
{
private:
	struct UIButtonDrawerData
	{
		UIStyle panelStyle;
		Core::TRange<Fixed> btnNPosRange = {Fixed(-0.85f), Fixed(0.85f)};
		bool bHorizontal = false;

		UIButtonDrawerData();
	};

private:
	UIButtonDrawerData m_data;
	Vec<UIButton*> m_uiButtons;

public:
	UIButtonDrawer();
	~UIButtonDrawer() override;

protected:
	void OnCreated() override;

public:
	// Returns false if any button already added - will be vertical
	bool SetHorizontal(bool bHorizontal = true);
	UIButton::OnClick::Token AddButton(UIText buttonText, UIButton::OnClick::Callback onInteracted, UIButton** ppButton = nullptr);
	void SetBaseStyle(UIStyle style, Vector2 nPosition = Vector2::Zero);

private:
	void SetButtonPositions();
};
} // namespace LittleEngine
