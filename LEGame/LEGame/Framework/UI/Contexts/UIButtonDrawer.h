#pragma once
#include "Core/CoreTypes.h"
#include "Core/TRange.h"
#include "LEGame/GameModel/UI/UIContext.h"
#include "LEGame/GameModel/UI/UIStyle.h"
#include "LEGame/Framework/UI/Widgets/UIButton.h"

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

	// Returns false if any button already added - will be vertical
	bool SetHorizontal(bool bHorizontal = true);
	UIButtonDrawer* SetPanel(UIStyle panelStyle);
	UIButton::OnClick::Token AddButton(UIText buttonText,
									   UIButton::OnClick::Callback onInteracted,
									   UIButton** ppButton = nullptr);

protected:
	void OnCreated() override;

private:
	void SetButtonPositions();
};
} // namespace LittleEngine
