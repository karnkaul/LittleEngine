#include "le_stdafx.h"
#include "UIDialogue.h"

namespace LittleEngine {
	namespace {
		const Fixed headerHeight = 80;
		const Fixed footerHeight = 80;
	}

	UIDialogue::UIDialogue() : UIContext("UIDialogue") {}
	UIDialogue::UIDialogue(const std::string& name) : UIContext(name) {}
	UIDialogue::~UIDialogue() = default;

	Delegate::Token UIDialogue::InitDialogue(UIDialogueData && data, UIText mainButtonUIText, Delegate::Callback OnMainButton) {
		m_data = std::move(data);
		const Fixed contentHeight = m_data.size.y - (headerHeight + footerHeight);
		m_bAutoDestroyOnCancel = m_data.bDestroyOnReturn;
		m_pRootElement->m_transform.size = { m_data.size.x, m_data.size.y };
		m_pRootElement->SetPanel(Colour::White);

		m_pHeader = AddElement<UIElement>(GetName() + " Header");
		m_pHeader->m_transform.size = { m_data.size.x, headerHeight };
		m_pHeader->m_transform.SetAutoPadNPosition({ 0, Fixed::One });
		m_pHeader->SetPanel(m_data.headerBG);
		m_pHeader->SetText(m_data.titleUIText);
		
		m_pContent = AddElement<UIElement>(GetName() + " Content");
		m_pContent->m_transform.size = { m_data.size.x, contentHeight };
		m_pContent->m_transform.nPosition = { 0, Fixed::One };
		m_pContent->m_transform.pixelPad = { 0, -(headerHeight + contentHeight * Fixed::OneHalf) };
		m_pContent->SetPanel(m_data.contentBG);
		m_pContent->SetText(m_data.contentUIText);
		
		m_pFooter = AddElement<UIElement>(GetName() + " Footer");
		m_pFooter->m_transform.size = { m_data.size.x, footerHeight };
		m_pFooter->m_transform.SetAutoPadNPosition({ 0, -Fixed::One });

		m_pMainButton = AddWidget<UIButton>("Dialog Button 0");
		UIButtonData buttonData = m_data.buttonData;
		m_pMainButton->InitButton(std::move(buttonData));
		m_pMainButton->SetText(mainButtonUIText);
		UIElement* pButtonEl = m_pMainButton->GetButtonElement();
		pButtonEl->m_transform.SetParent(m_pFooter->m_transform);
		return m_pMainButton->AddCallback(OnMainButton);
	}

	Delegate::Token UIDialogue::AddOtherButton(UIText otherButtonUIText, Delegate::Callback OnOtherButton, bool bSelect) {
		if (m_pOtherButton) {
			Logger::Log(*this, "Other button already exists on this UIDialogue! Ignoring call to AddOtherButton", Logger::Severity::Warning);
			return nullptr;
		}
		m_pOtherButton = AddWidget<UIButton>("Dialog Button 0", true);
		UIButtonData buttonData = m_data.buttonData;
		m_pOtherButton->InitButton(std::move(buttonData));
		m_pOtherButton->SetText(otherButtonUIText);
		UIElement* pOtherButtonEl = m_pOtherButton->GetButtonElement();
		pOtherButtonEl->m_transform.SetParent(m_pFooter->m_transform);
		pOtherButtonEl->m_transform.nPosition = { Fixed::OneHalf, 0 };

		UIElement* pMainButtonEl = m_pMainButton->GetButtonElement();
		pMainButtonEl->m_transform.SetParent(m_pFooter->m_transform);
		pMainButtonEl->m_transform.nPosition = { -Fixed::OneHalf, 0 };

		// Terrible hack
		if (!bSelect) OnLeft();

		return m_pOtherButton->AddCallback(OnOtherButton);
	}
}
