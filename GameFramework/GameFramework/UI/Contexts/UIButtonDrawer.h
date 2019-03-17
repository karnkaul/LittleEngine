#pragma once
#include "LittleEngine/UI/UIContext.h"
#include "LittleEngine/UI/UIStyle.h"
#include "GameFramework/UI/Widgets/UIButton.h"
#include "CoreTypes.h"

namespace LittleEngine
{
class UIButtonDrawer : public UIContext
{
private:
	struct UIButtonDrawerData
	{
		UIStyle panelStyle;
		TRange<Fixed> btnNPosRange = {Fixed(-0.85f), Fixed(0.85f)};
		bool bHorizontal = false;

		UIButtonDrawerData();
	};

private:
	UIButtonDrawerData m_data;
	Vec<UIButton*> m_uiButtons;

public:
	UIButtonDrawer();
	UIButtonDrawer(const String& name);
	virtual ~UIButtonDrawer();

	UIButtonDrawer* SetPanel(const UIStyle& panelStyle);
	UIButton::OnClick::Token AddButton(const UIText& buttonText, const UIButton::OnClick::Callback& OnInteracted, UIButton** ppButton = nullptr);

protected:
	virtual void OnInitContext() override;

private:
	void SetButtonPositions();
};
} // namespace LittleEngine
