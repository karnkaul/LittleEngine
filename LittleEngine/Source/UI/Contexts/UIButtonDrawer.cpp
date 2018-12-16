#include "le_stdafx.h"
#include "UIButtonDrawer.h"

namespace LittleEngine {
	UIButtonDrawer::UIButtonDrawer() : UIContext("UIButtonDrawer") {}
	UIButtonDrawer::UIButtonDrawer(const std::string & name) : UIContext(name) {}
	UIButtonDrawer::~UIButtonDrawer() = default;

	void UIButtonDrawer::InitButtonDrawer(const UIButtonDrawerData & data) {
		if (!m_init) {
			m_data = data;
			m_pRoot = AddElement<UIElement>(GetName() + " Root");
			m_pRoot->m_transform.size = m_data.panelSize;
			m_pRoot->SetPanel(m_data.panelColour, m_data.panelBorder, m_data.panelOutline);
			m_init = true;
			m_bAutoDestroyOnCancel = m_data.bDestroyOnReturn;
		}
	}

	Delegate::Token UIButtonDrawer::AddButton(const UIText & buttonText, Delegate::Callback OnInteracted) {
		if (!m_init) {
			Logger::Log(*this, "Cannot Add Button to uninitialised ButtonDrawer!", Logger::Severity::Error);
			return nullptr;
		}
		std::string buttonName = "Button" + Strings::ToString(m_uiButtons.size());
		UIButton* pButton = AddWidget<UIButton>(buttonName);
		UIButtonData buttonData = m_data.defaultButtonData;
		pButton->InitButton(std::move(buttonData));
		pButton->SetText(buttonText);
		pButton->GetButtonElement()->m_transform.SetParent(m_pRoot->m_transform);
		m_uiButtons.push_back(pButton);
		SetButtonPositions();
		return pButton->AddCallback(OnInteracted);
	}

	void UIButtonDrawer::SetButtonPositions() {
		int count = static_cast<int>(m_uiButtons.size());
		// Special cases
		if (count == 1) {
			m_uiButtons[0]->GetButtonElement()->m_transform.SetAutoPadNPosition({ 0, 0 });
			return;
		}
		if (count == 2) {
			Fixed midPoint = (m_data.btnYPosRange.max + m_data.btnYPosRange.min) * Fixed::OneHalf;
			Fixed upperMid = (m_data.btnYPosRange.max + midPoint) * Fixed::OneHalf;
			Fixed lowerMid = (m_data.btnYPosRange.min + midPoint) * Fixed::OneHalf;
			m_uiButtons[0]->GetButtonElement()->m_transform.nPosition = { 0, upperMid };
			m_uiButtons[1]->GetButtonElement()->m_transform.nPosition = { 0, lowerMid };
			return;
		}

		Fixed step = Fixed(m_data.btnYPosRange.max - m_data.btnYPosRange.min) / (count - 1);
		Fixed current = m_data.btnYPosRange.max;
		for (auto pButton : m_uiButtons) {
			pButton->GetButtonElement()->m_transform.nPosition = { Fixed::Zero, current };
			current -= step;
		}
	}
}
