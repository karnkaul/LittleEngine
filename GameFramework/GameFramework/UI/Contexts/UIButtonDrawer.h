#pragma once
#include "LittleEngine/UI/UIContext.h"
#include "GameFramework/UI/Widgets/UIButton.h"
#include "CoreTypes.h"

namespace LittleEngine
{
struct UIButtonDrawerData
{
	UIButtonData defaultButtonData;
	Vector2 panelSize;
	TRange<Fixed> btnNPosRange = {Fixed(-0.85f), Fixed(0.85f)};
	Fixed panelBorder = Fixed::Zero;
	Colour panelColour = Colour::White;
	Colour panelOutline = Colour::Transparent;
	bool bHorizontal = false;
	bool bDestroyOnReturn = true;

	static const UIButtonDrawerData DebugButtonDrawer(bool bModal = false);
	static UIButtonDrawerData CreateDrawer(bool bModal,
										   const Vector2& size,
										   Colour background,
										   UIButtonData* pButtonData = nullptr);
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
	Delegate::Token AddButton(const UIText& buttonText,
							  Delegate::Callback OnInteracted,
							  const UIButtonData* pButtonData = nullptr);

private:
	void SetButtonPositions();
};
} // namespace LittleEngine
