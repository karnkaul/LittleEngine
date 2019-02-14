#include "le_stdafx.h"
#include "UIController.h"
#include "Levels/Level.h"
#include "Engine/Logger/Logger.h"

namespace LittleEngine {
	UIController::UIController(Level & level) : UIObject(level.GetName() + "-UIController"), m_pLevel(&level) {
		Logger::Log(*this, GetNameInBrackets() + " constructed");
	}

	UIController::~UIController() {
		Logger::Log(*this, GetNameInBrackets() + " destroyed");
	}

	UIContext* UIController::GetActiveContext() const {
		return m_uActiveContext.get();
	}

	void UIController::Tick(const Fixed & deltaMS) {
		if (m_uActiveContext) {
			m_uActiveContext->Tick(deltaMS);
			if (m_uActiveContext->m_bDestroyed) m_uActiveContext = nullptr;
		}
	}

	void UIController::Render() {
		if (m_uActiveContext) m_uActiveContext->Render();
	}
}
