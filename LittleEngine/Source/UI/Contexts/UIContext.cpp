#include "le_stdafx.h"
#include "UIContext.h"
#include "Levels/Level.h"
#include "Engine/Logger/Logger.h"

namespace LittleEngine {
	UIContext::UIContext() : UIObject("Untitled") {
		Logger::Log(*this, "Untitled UIContext constructed", Logger::Severity::Debug);
	}

	UIContext::UIContext(const std::string & name) : UIObject(name) {
		Logger::Log(*this, GetNameInBrackets() + " UIContext constructed", Logger::Severity::Debug);
	}

	UIContext::~UIContext() {
		SetActive(false);
		m_uiWidgets.Clear();
		m_uiElements.clear();
		Logger::Log(*this, GetNameInBrackets() + " UIContext destroyed", Logger::Severity::Debug);
	}

	void UIContext::InitContext(Level & level) {
		this->m_pLevel = &level;
		m_pRootElement = AddElement<UIElement>(GetName() + " Root");
	}

	void UIContext::SetActive(bool bActive) {
		m_inputTokens.clear();
		if (bActive) {
			ResetSelection();
			InputHandler& input = m_pLevel->GetInputHandler();
			m_inputTokens.push_back(input.Register(GameInput::Enter, std::bind(&UIContext::OnEnterPressed, this), OnKey::Pressed, true));
			m_inputTokens.push_back(input.Register(GameInput::Enter, std::bind(&UIContext::OnEnterReleased, this), OnKey::Released, true));
			m_inputTokens.push_back(input.Register(GameInput::Return, std::bind(&UIContext::OnReturnReleased, this), OnKey::Released, true));
			m_inputTokens.push_back(input.Register(GameInput::Up, std::bind(&UIContext::OnUp, this), OnKey::Released, true));
			m_inputTokens.push_back(input.Register(GameInput::Down, std::bind(&UIContext::OnDown, this), OnKey::Released, true));
			m_inputTokens.push_back(input.Register(GameInput::Left, std::bind(&UIContext::OnLeft, this), OnKey::Released, true));
			m_inputTokens.push_back(input.Register(GameInput::Right, std::bind(&UIContext::OnRight, this), OnKey::Released, true));

			// Block navigation
			m_inputTokens.push_back(input.Register(GameInput::Enter, std::bind(&UIContext::Discard, this), OnKey::Held, true));
			m_inputTokens.push_back(input.Register(GameInput::Return, std::bind(&UIContext::Discard, this), OnKey::Held, true));
			m_inputTokens.push_back(input.Register(GameInput::Up, std::bind(&UIContext::Discard, this), OnKey::Held, true));
			m_inputTokens.push_back(input.Register(GameInput::Down, std::bind(&UIContext::Discard, this), OnKey::Held, true));
			m_inputTokens.push_back(input.Register(GameInput::Left, std::bind(&UIContext::Discard, this), OnKey::Held, true));
			m_inputTokens.push_back(input.Register(GameInput::Right, std::bind(&UIContext::Discard, this), OnKey::Held, true));
		}
	}

	void UIContext::ResetSelection() {
		m_uiWidgets.Reset(true);
		UIWidget* pSelected = GetSelected();
		if (pSelected) pSelected->OnSelected();
	}

	UIWidget* UIContext::GetSelected() {
		return m_uiWidgets.Get();
	}

	UIElement* UIContext::GetRootElement() const {
		return m_pRootElement;
	}

	OnInput::Token UIContext::SetOnCancelled(OnInput::Callback Callback, bool bAutoDestroy) {
		m_bAutoDestroyOnCancel = bAutoDestroy;
		return m_onCancelledDelegate.Register(Callback);
	}

	void UIContext::Destruct() {
		OnDestroying();
		m_bDestroyed = true;
	}

	void UIContext::Tick(const Fixed & deltaMS) {
		if (m_bDestroyed) return;
		for (auto& uElement : m_uiElements) {
			uElement->Tick(deltaMS);
		}
		m_uiWidgets.ForEach([&deltaMS](UIContext::UUIWidget& uUIWidget) { uUIWidget->Tick(deltaMS); });
	}

	void UIContext::Render() {
		if (m_bDestroyed) return;
		for (auto& uElement : m_uiElements) {
			uElement->Render();
		}
		m_uiWidgets.ForEach([](UIContext::UUIWidget& uUIWidget) { uUIWidget->Render(); });
	}

	void UIContext::OnDestroying() {}

	void UIContext::OnUp() {
		if (m_uiWidgets.CurrentVecCount() < 2) return;
		UIWidget* pSelected = GetSelected();
		if (pSelected) pSelected->OnDeselected();
		m_uiWidgets.Up();
		pSelected = GetSelected();
		if (pSelected) pSelected->OnSelected();
	}

	void UIContext::OnDown() {
		if (m_uiWidgets.CurrentVecCount() < 2) return;
		UIWidget* pSelected = GetSelected();
		if (pSelected) pSelected->OnDeselected();
		m_uiWidgets.Down();
		pSelected = GetSelected();
		if (pSelected) pSelected->OnSelected();
	}

	void UIContext::OnLeft() {
		if (m_uiWidgets.NumColumns() < 2) return;
		UIWidget* pSelected = GetSelected();
		if (pSelected) pSelected->OnDeselected();
		m_uiWidgets.Left();
		pSelected = GetSelected();
		if (pSelected) pSelected->OnSelected();
	}

	void UIContext::OnRight() {
		if (m_uiWidgets.NumColumns() < 2) return;
		UIWidget* pSelected = GetSelected();
		if (pSelected) pSelected->OnDeselected();
		m_uiWidgets.Right();
		pSelected = GetSelected();
		if (pSelected) pSelected->OnSelected();
	}

	void UIContext::OnEnterPressed() {
		UIWidget* pSelected = GetSelected();
		if (pSelected) {
			pSelected->OnInteractStart();
		}
	}

	void UIContext::OnEnterReleased() {
		UIWidget* pSelected = GetSelected();
		if (pSelected) {
			pSelected->OnInteractEnd();
		}
	}

	void UIContext::OnReturnReleased() {
		m_onCancelledDelegate();
		m_bDestroyed = m_bAutoDestroyOnCancel;
	}

	void UIContext::Discard() {}
}
