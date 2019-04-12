#pragma once
#include "CoreTypes.h"
#include "TRange.hpp"
#include "LittleEngine/UI/UIContext.h"
#include "LittleEngine/UI/UIStyle.h"
#include "LittleEngine/UI/UIText.h"
#include "GameFramework/UI/Widgets/UIButton.h"

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
	UIButtonDrawer(String name);
	~UIButtonDrawer() override;

	// Returns false if any button already added - will be vertical
	bool SetHorizontal(bool bHorizontal = true);
	UIButtonDrawer* SetPanel(UIStyle panelStyle);
	UIButton::OnClick::Token AddButton(UIText buttonText,
									   UIButton::OnClick::Callback onInteracted,
									   UIButton** ppButton = nullptr);

protected:
	void OnInitContext() override;

private:
	void SetButtonPositions();
};
} // namespace LittleEngine
