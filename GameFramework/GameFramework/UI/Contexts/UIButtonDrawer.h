#pragma once
#include "LittleEngine/UI/UIContext.h"
#include "LittleEngine/UI/UIStyle.h"
#include "GameFramework/UI/Widgets/UIButton.h"
#include "CoreTypes.h"

namespace LittleEngine
{
struct UIButtonDrawerData
{
	UIStyle panelStyle;
	TRange<Fixed> btnNPosRange = {Fixed(-0.85f), Fixed(0.85f)};
	bool bHorizontal = false;
	bool bDestroyOnReturn = true;

	static const UIButtonDrawerData DebugButtonDrawer(bool bModal = false);
	static UIButtonDrawerData CreateDrawer(bool bModal, const Vector2& size, Colour background);
};

class UIButtonDrawer : public UIContext
{
private:
	UIButtonDrawerData m_data;
	Vector<UIButton*> m_uiButtons;
	bool m_init = false;

public:
	UIButtonDrawer();
	UIButtonDrawer(const String& name);
	virtual ~UIButtonDrawer();

	void InitButtonDrawer(const UIButtonDrawerData& data);
	OnClick::Token AddButton(const UIText& buttonText, OnClick::Callback OnInteracted);

private:
	void SetButtonPositions();
};
} // namespace LittleEngine
