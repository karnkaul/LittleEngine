#include "stdafx.h"
#include "UIContext.h"
#include "LittleEngine/World/World.h"
#include "Logger.h"

namespace LittleEngine
{
UIContext::UIContext() : UIObject("Untitled")
{
	LogD("Untitled UIContext constructed");
}

UIContext::UIContext(const String& name) : UIObject(name)
{
	LogD(LogName() + " UIContext constructed");
}

UIContext::~UIContext()
{
	SetActive(false);
	m_uiWidgets.Clear();
	m_uiElements.clear();
	LogD(LogName() + " UIContext destroyed");
}

void UIContext::InitContext()
{
	m_pRootElement = AddElement<UIElement>(m_name + " Root");
	SetActive(true);
}

void UIContext::SetActive(bool bActive)
{
	m_inputTokens.clear();
	if (bActive)
	{
		ResetSelection();
		Tick(Time::Zero);
		m_inputTokens.push_back(Services::Engine()->Input()->Register(std::bind(&UIContext::OnInput, this, _1)));
	}
}

void UIContext::ResetSelection()
{
	m_uiWidgets.Reset(true);
	UIWidget* pSelected = GetSelected();
	if (pSelected)
		pSelected->OnSelected();
}

UIWidget* UIContext::GetSelected()
{
	return m_uiWidgets.Get();
}

UIElement* UIContext::GetRootElement() const
{
	return m_pRootElement;
}

Core::Delegate<>::Token UIContext::SetOnCancelled(Core::Delegate<>::Callback Callback, bool bAutoDestroy)
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
		pSelected->OnDeselected();
	m_uiWidgets.Up();
	pSelected = GetSelected();
	if (pSelected)
		pSelected->OnSelected();
}

void UIContext::OnDown()
{
	if (m_uiWidgets.CurrentVecCount() < 2)
		return;
	UIWidget* pSelected = GetSelected();
	if (pSelected)
		pSelected->OnDeselected();
	m_uiWidgets.Down();
	pSelected = GetSelected();
	if (pSelected)
		pSelected->OnSelected();
}

void UIContext::OnLeft()
{
	if (m_uiWidgets.NumColumns() < 2)
		return;
	UIWidget* pSelected = GetSelected();
	if (pSelected)
		pSelected->OnDeselected();
	m_uiWidgets.Left();
	pSelected = GetSelected();
	if (pSelected)
		pSelected->OnSelected();
}

void UIContext::OnRight()
{
	if (m_uiWidgets.NumColumns() < 2)
		return;
	UIWidget* pSelected = GetSelected();
	if (pSelected)
		pSelected->OnDeselected();
	m_uiWidgets.Right();
	pSelected = GetSelected();
	if (pSelected)
		pSelected->OnSelected();
}

void UIContext::OnEnterPressed()
{
	UIWidget* pSelected = GetSelected();
	if (pSelected)
	{
		pSelected->OnInteractStart();
	}
}

void UIContext::OnEnterReleased()
{
	UIWidget* pSelected = GetSelected();
	if (pSelected)
	{
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
