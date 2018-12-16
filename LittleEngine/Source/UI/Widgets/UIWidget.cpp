#include "le_stdafx.h"
#include "UIWidget.h"
#include "Engine/Logger/Logger.h"

namespace LittleEngine {
	UIWidget::UIWidget() : UIObject("Untitled") {
		Logger::Log(*this, "Untitled UIWidget constructed", Logger::Severity::Debug);
	}

	UIWidget::UIWidget(const std::string & name) : UIObject(name) {
		Logger::Log(*this, GetNameInBrackets() + " UIWidget constructed", Logger::Severity::Debug);
	}

	UIWidget::~UIWidget() {
		Logger::Log(*this, GetNameInBrackets() + " UIWidget destroyed", Logger::Severity::Debug);
	}

	void UIWidget::InitWidget(Level & level, UIContext& owner) {
		this->m_pLevel = &level;
		this->m_pOwner = &owner;
	}

	void UIWidget::Tick(const Fixed & deltaMS) {
		if (m_bDestroyed) return;
		for (auto& uUIelement : m_uiElements) {
			uUIelement->Tick(deltaMS);
		}
	}

	void UIWidget::Render() {
		if (m_bDestroyed) return;
		for (auto& uUIElement : m_uiElements) {
			uUIElement->Render();
		}
	}
}
