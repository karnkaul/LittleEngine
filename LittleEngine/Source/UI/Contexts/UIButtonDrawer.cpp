#include "le_stdafx.h"
#include "UIButtonDrawer.h"

namespace LittleEngine {
	const UIButtonDrawerData UIButtonDrawerData::DebugButtonDrawer(bool bModal) {
		UIButtonDrawerData data;
		data.defaultButtonData = UIButtonData::DebugButton();
		data.bDestroyOnReturn = !bModal;
		data.panelSize = { 600, 500 };
		data.panelColour = Colour(100, 100, 100, 100);
		return data;
	}

	UIButtonDrawerData UIButtonDrawerData::CreateDrawer(bool bModal, const Vector2& size, Colour background, UIButtonData* pButtonData) {
		UIButtonDrawerData data;
		data.defaultButtonData = pButtonData ? *pButtonData : UIButtonData::DebugButton();
		data.bDestroyOnReturn = !bModal;
		data.panelSize = size;
		data.panelColour = background;
		return data;
	}

	UIButtonDrawer::UIButtonDrawer() : UIContext("UIButtonDrawer") {}
	UIButtonDrawer::UIButtonDrawer(const std::string & name) : UIContext(name) {}
	UIButtonDrawer::~UIButtonDrawer() = default;

	void UIButtonDrawer::InitButtonDrawer(const UIButtonDrawerData & data) {
		if (!m_init) {
			m_data = data;
			m_pRootElement->m_transform.size = m_data.panelSize;
			m_pRootElement->SetPanel(m_data.panelColour, m_data.panelBorder, m_data.panelOutline);
			m_init = true;
			m_bAutoDestroyOnCancel = m_data.bDestroyOnReturn;
		}
	}

	Delegate::Token UIButtonDrawer::AddButton(const UIText & buttonText, Delegate::Callback OnInteracted, const UIButtonData* pButtonData) {
		if (!m_init) {
			Logger::Log(*this, "Cannot Add Button to uninitialised ButtonDrawer!", Logger::Severity::Error);
			return nullptr;
		}
		if (!pButtonData) pButtonData = &m_data.defaultButtonData;
		std::string buttonName = "Button" + Strings::ToString(m_uiButtons.size());
		UIButton* pButton = AddWidget<UIButton>(buttonName, m_data.bHorizontal);
		UIButtonData buttonData = *pButtonData;
		pButton->InitButton(std::move(buttonData));
		pButton->SetText(buttonText);
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
			Fixed midPoint = (m_data.btnNPosRange.max + m_data.btnNPosRange.min) * Fixed::OneHalf;
			Fixed upperMid = (m_data.btnNPosRange.max + midPoint) * Fixed::OneHalf;
			Fixed lowerMid = (m_data.btnNPosRange.min + midPoint) * Fixed::OneHalf;
			Vector2 upper = m_data.bHorizontal ? Vector2(lowerMid, 0) : Vector2(0, upperMid);
			Vector2 lower = m_data.bHorizontal ? Vector2(upperMid, 0) : Vector2(0, lowerMid);
			m_uiButtons[0]->GetButtonElement()->m_transform.nPosition = upper;
			m_uiButtons[1]->GetButtonElement()->m_transform.nPosition = lower;
			return;
		}

		// [+Y -> -Y] or [-X -> +X]
		Fixed step = Fixed(m_data.btnNPosRange.max - m_data.btnNPosRange.min) / (count - 1);
		Fixed current = m_data.bHorizontal ? m_data.btnNPosRange.min : m_data.btnNPosRange.max;
		for (auto pButton : m_uiButtons) {
			Vector2 pos = m_data.bHorizontal ? Vector2(current, 0) : Vector2(0, current);
			pButton->GetButtonElement()->m_transform.nPosition = pos;
			current += (m_data.bHorizontal ? step : -step);
		}
	}
}
