#pragma once
#include "UIContext.h"
#include "../Widgets/UIButton.h"
#include "TRange.hpp"

namespace LittleEngine {
	template<typename T>
	using TRange = GameUtils::TRange<T>;

	struct UIButtonDrawerData {
		UIButtonData defaultButtonData;
		Vector2 panelSize;
		TRange<Fixed> btnYPosRange = { Fixed(-0.85f), Fixed(0.85f) };
		Fixed panelBorder = Fixed::Zero;
		Colour panelColour = Colour::White;
		Colour panelOutline = Colour::Transparent;
		bool bDestroyOnReturn = true;
	};

	class UIButtonDrawer : public UIContext {
	private:
		UIButtonDrawerData m_data;
		std::vector<UIButton*> m_uiButtons;
		UIElement* m_pRoot = nullptr;
		bool m_init = false;

	public:
		UIButtonDrawer();
		UIButtonDrawer(const std::string& name);
		virtual ~UIButtonDrawer();

		void InitButtonDrawer(const UIButtonDrawerData& data);
		Delegate::Token AddButton(const UIText& buttonText, Delegate::Callback OnInteracted);

	private:
		void SetButtonPositions();
	};
}
