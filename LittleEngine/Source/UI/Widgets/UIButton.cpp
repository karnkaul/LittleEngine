#include "le_stdafx.h"
#include "UIButton.h"
#include "Engine/Logger/Logger.h"

namespace LittleEngine {
	const UIButtonData UIButtonData::DebugButton() {
		UIButtonData data;
		data.size = { 200, 60 };
		data.interactingFill = { 255, 150, 255, 255 };
		data.selectedFill = { 255, 135, 69, 255 };
		data.notSelectedFill = { 255, 135, 69, 255 };
		data.selectedBorder = 5;
		data.selectedOutline = { 18, 139, 165, 255 };
		return data;
	}

	UIButton::UIButton() : UIWidget("Untitled") {
		Logger::Log(*this, "Untitled UIButton constructed", Logger::Severity::Debug);
	}

	UIButton::UIButton(const std::string& name) : UIWidget(name) {
		Logger::Log(*this, GetNameInBrackets() + " constructed", Logger::Severity::Debug);
	}

	UIButton::~UIButton() {
		Logger::Log(*this, GetNameInBrackets() + " destroyed", Logger::Severity::Debug);
	}

	void UIButton::InitButton(UIButtonData&& data) {
		m_data = std::move(data);
		m_pUIElement = AddElement<UIElement>();
		m_pUIElement->m_transform.size = m_data.size;
		m_pUIElement->SetText(m_name);
		m_pUIElement->SetPanel(m_data.notSelectedFill);
	}

	void UIButton::SetText(const UIText& uiText) {
		m_pUIElement->SetText(uiText);
	}

	Delegate::Token UIButton::AddCallback(Delegate::Callback Callback) {
		return m_OnInteracted.Register(Callback);
	}

	UIElement* UIButton::GetButtonElement() const {
		return m_pUIElement;
	}

	void UIButton::OnSelected() {
		m_bSelected = true;
		if (m_pUIElement) {
			m_pUIElement->SetPanel(m_data.selectedFill, m_data.selectedBorder, m_data.selectedOutline);
		}
	}

	void UIButton::OnDeselected() {
		m_bSelected = false;
		if (m_pUIElement) {
			m_pUIElement->SetPanel(m_data.notSelectedFill, m_data.notSelectedBorder, m_data.notSelectedOutline);
		}
	}

	void UIButton::OnInteractStart() {
		if (m_pUIElement) {
			m_pUIElement->SetPanel(m_data.interactingFill);
		}
	}

	void UIButton::OnInteractEnd() {
		if (m_pUIElement) {
			if (m_bSelected) m_pUIElement->SetPanel(m_data.selectedFill, m_data.selectedBorder, m_data.selectedOutline);
			else m_pUIElement->SetPanel(m_data.notSelectedFill, m_data.notSelectedBorder, m_data.notSelectedOutline);
		}
		m_OnInteracted();
	}
}
