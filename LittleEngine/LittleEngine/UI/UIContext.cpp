#include "stdafx.h"
#include "UIContext.h"
#include "LittleEngine/Game/World/World.h"
#include "Logger.h"

namespace LittleEngine
{
UIContext::UIContext() : UIObject("Untitled")
{
	SetName("", "UIContext");
}

UIContext::UIContext(const String& name) : UIObject(name)
{
	SetName("", "UIContext");
}

UIContext::~UIContext()
{
	SetActive(false);
	m_uiWidgets.Clear();
	m_uiElements.clear();
	LOG_D("%s destroyed", LogNameStr());
}

void UIContext::InitContext(LayerID rootLayer)
{
	m_pRootElement = AddElement<UIElement>(String(GetNameStr()) + " Root");
	m_pRootElement->m_layer = rootLayer;
	OnInitContext();
	SetActive(true);
}

void UIContext::SetActive(bool bActive, bool bResetSelection)
{
	m_inputTokens.clear();
	if (bActive)
	{
		if (bResetSelection)
			ResetSelection();
		Tick(Time::Zero);
		m_inputTokens.push_back(Services::Engine()->Input()->Register(std::bind(&UIContext::OnInput, this, _1)));
	}
}

void UIContext::ResetSelection()
{
	m_uiWidgets.Reset(true);
	m_uiWidgets.ForEach([](UPtr<UIWidget>& uWidget) { uWidget->OnDeselected(); });
	UIWidget* pSelected = GetSelected();
	if (pSelected)
	{
		pSelected->m_state = UIWidgetState::Selected;
		pSelected->OnSelected();
	}
}

UIWidget* UIContext::GetSelected()
{
	return m_uiWidgets.Get();
}

UIElement* UIContext::GetRootElement() const
{
	return m_pRootElement;
}

UIContext::OnCancelled::Token UIContext::SetOnCancelled(const OnCancelled::Callback& Callback, bool bAutoDestroy)
{
	m_bAutoDestroyOnCancel = bAutoDestroy;
	return m_onCancelledDelegate.Register(Callback);
}

void UIContext::Destruct()
{
	OnDestroying();
	m_bDestroyed = true;
}

void UIContext::Tick(Time dt)
{
	if (m_bDestroyed)
		return;
	for (auto& uElement : m_uiElements)
	{
		uElement->Tick(dt);
	}
	m_uiWidgets.ForEach([dt](UIContext::UUIWidget& uUIWidget) { uUIWidget->Tick(dt); });
}

void UIContext::OnInitContext()
{
}

void UIContext::OnDestroying()
{
}

bool UIContext::OnInput(const EngineInput::Frame& frame)
{
	if (m_bDestroyed)
		return false;

	if (frame.IsPressed(GameInputType::Enter))
		OnEnterPressed();
	if (frame.IsReleased(GameInputType::Enter))
		OnEnterReleased();
	if (frame.IsReleased(GameInputType::Back))
		OnBackReleased();
	if (frame.IsReleased(GameInputType::Up))
		OnUp();
	if (frame.IsReleased(GameInputType::Down))
		OnDown();
	if (frame.IsReleased(GameInputType::Left))
		OnLeft();
	if (frame.IsReleased(GameInputType::Right))
		OnRight();
	return true;
}

void UIContext::OnUp()
{
	if (m_uiWidgets.CurrentVecCount() < 2)
		return;
	UIWidget* pSelected = GetSelected();
	if (pSelected)
	{
		pSelected->m_state = UIWidgetState::NotSelected;
		pSelected->OnDeselected();
	}
	m_uiWidgets.Up();
	pSelected = GetSelected();
	if (pSelected)
	{
		pSelected->m_state = UIWidgetState::Selected;
		pSelected->OnSelected();
	}
}

void UIContext::OnDown()
{
	if (m_uiWidgets.CurrentVecCount() < 2)
		return;
	UIWidget* pSelected = GetSelected();
	if (pSelected)
	{
		pSelected->m_state = UIWidgetState::NotSelected;
		pSelected->OnDeselected();
	}

	m_uiWidgets.Down();
	pSelected = GetSelected();
	if (pSelected)
	{
		pSelected->m_state = UIWidgetState::Selected;
		pSelected->OnSelected();
	}
}

void UIContext::OnLeft()
{
	if (m_uiWidgets.NumColumns() < 2)
		return;
	UIWidget* pSelected = GetSelected();
	if (pSelected)
	{
		pSelected->m_state = UIWidgetState::NotSelected;
		pSelected->OnDeselected();
	}

	m_uiWidgets.Left();
	pSelected = GetSelected();
	if (pSelected)
	{
		pSelected->m_state = UIWidgetState::Selected;
		pSelected->OnSelected();
	}
}

void UIContext::OnRight()
{
	if (m_uiWidgets.NumColumns() < 2)
		return;
	UIWidget* pSelected = GetSelected();
	if (pSelected)
	{
		pSelected->m_state = UIWidgetState::NotSelected;
		pSelected->OnDeselected();
	}

	m_uiWidgets.Right();
	pSelected = GetSelected();
	if (pSelected)
	{
		pSelected->m_state = UIWidgetState::Selected;
		pSelected->OnSelected();
	}
}

void UIContext::OnEnterPressed()
{
	UIWidget* pSelected = GetSelected();
	if (pSelected)
	{
		pSelected->m_state = UIWidgetState::Interacting;
		pSelected->OnInteractStart();
	}
}

void UIContext::OnEnterReleased()
{
	UIWidget* pSelected = GetSelected();
	if (pSelected)
	{
		pSelected->m_state = UIWidgetState::Selected;
		pSelected->OnInteractEnd();
	}
}

void UIContext::OnBackReleased()
{
	m_onCancelledDelegate();
	m_bDestroyed = m_bAutoDestroyOnCancel;
}

void UIContext::Discard()
{
}
} // namespace LittleEngine
