#include "stdafx.h"
#include "Logger.h"
#include "UIContext.h"
#include "LittleEngine/Engine/EngineService.h"
#include "LittleEngine/Game/World/World.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/UI/UIElement.h"
#include "LittleEngine/UI/UIWidget.h"
#include "LittleEngine/UI/UIWidgetMatrix.h"

namespace LittleEngine
{
UIContext::UIContext() : UIObject("Untitled")
{
	SetName("", "UIContext");
}

UIContext::UIContext(String name) : UIObject(std::move(name))
{
	SetName("", "UIContext");
}

UIContext::~UIContext()
{
	SetActive(false);
	m_uUIWidgets->Clear();
	m_uiElements.clear();
	LOG_D("%s destroyed", LogNameStr());
}

void UIContext::InitContext(LayerID rootLayer)
{
	m_uUIWidgets = MakeUnique<UIWidgetMatrix>();
	m_pRootElement = AddElement<UIElement>(String(GetNameStr()) + " Root");
	m_pRootElement->m_layer = rootLayer;
	OnInitContext();
	SetActive(true);
}

LayerID UIContext::GetMaxLayer() const
{
	LayerID maxLayer = m_pRootElement->m_layer;
	for (const auto& uElement : m_uiElements)
	{
		if (uElement->m_layer > maxLayer)
		{
			maxLayer = uElement->m_layer;
		}
	}
	m_uUIWidgets->ForEach([&maxLayer](const UPtr<UIWidget>& uWidget) 
	{
		if (uWidget->GetMaxLayer() > maxLayer)
		{
			maxLayer = uWidget->GetMaxLayer();
		}
	}
	);
	return maxLayer;
}

void UIContext::SetActive(bool bActive, bool bResetSelection)
{
	m_inputTokens.clear();
	if (bActive)
	{
		if (bResetSelection)
		{
			ResetSelection();
		}
		Tick(Time::Zero);
		m_inputTokens.push_back(Services::Engine()->Input()->Register(std::bind(&UIContext::OnInput, this, _1)));
	}
}

void UIContext::ResetSelection()
{
	m_uUIWidgets->Reset(true);
	m_uUIWidgets->ForEach([](UPtr<UIWidget>& uWidget) {
		if (uWidget->m_state != UIWidgetState::Uninteractable)
		{
			uWidget->OnDeselected();
		}
	});
	UIWidget* pSelected = GetSelected();
	if (pSelected && pSelected->IsInteractable())
	{
		pSelected->SetState(UIWidgetState::Selected);
		pSelected->OnSelected();
	}
}

UIWidget* UIContext::GetSelected()
{
	return m_uUIWidgets->Current();
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
	{
		return;
	}
	for (auto& uElement : m_uiElements)
	{
		uElement->Tick(dt);
	}
	m_uUIWidgets->ForEach([dt](UIContext::UUIWidget& uUIWidget) { uUIWidget->Tick(dt); });
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
	{
		return false;
	}

	if (frame.IsPressed(GameInputType::Enter))
	{
		OnEnterPressed();
	}
	if (frame.IsReleased(GameInputType::Enter))
	{
		OnEnterReleased(m_bInteracting);
	}
	if (frame.IsReleased(GameInputType::Back))
	{
		OnBackReleased();
	}
	if (frame.IsReleased(GameInputType::Up))
	{
		OnUp();
	}
	if (frame.IsReleased(GameInputType::Down))
	{
		OnDown();
	}
	if (frame.IsReleased(GameInputType::Left))
	{
		OnLeft();
	}
	if (frame.IsReleased(GameInputType::Right))
	{
		OnRight();
	}
	return true;
}

void UIContext::OnUp()
{
	if (m_bInteracting || m_uUIWidgets->CurrentVecCount() < 2)
	{
		return;
	}
	UIWidget* pSelected = GetSelected();
	if (pSelected && pSelected->IsInteractable())
	{
		pSelected->SetState(UIWidgetState::NotSelected);
		pSelected->OnDeselected();
	}
	pSelected = m_uUIWidgets->NextSelectableVertical(false);
	if (pSelected)
	{
		pSelected->SetState(UIWidgetState::Selected);
		pSelected->OnSelected();
	}
}

void UIContext::OnDown()
{
	if (m_bInteracting || m_uUIWidgets->CurrentVecCount() < 2)
	{
		return;
	}
	UIWidget* pWidget = GetSelected();
	if (pWidget && pWidget->IsInteractable())
	{
		pWidget->SetState(UIWidgetState::NotSelected);
		pWidget->OnDeselected();
	}
	pWidget = m_uUIWidgets->NextSelectableVertical(true);
	if (pWidget)
	{
		pWidget->SetState(UIWidgetState::Selected);
		pWidget->OnSelected();
	}
}

void UIContext::OnLeft()
{
	if (m_bInteracting || m_uUIWidgets->NumColumns() < 2)
	{
		return;
	}
	UIWidget* pWidget = GetSelected();
	if (pWidget && pWidget->IsInteractable())
	{
		pWidget->SetState(UIWidgetState::NotSelected);
		pWidget->OnDeselected();
	}
	pWidget = m_uUIWidgets->NextSelectableHorizontal(false);
	if (pWidget)
	{
		pWidget->SetState(UIWidgetState::Selected);
		pWidget->OnSelected();
	}
}

void UIContext::OnRight()
{
	if (m_bInteracting || m_uUIWidgets->NumColumns() < 2)
	{
		return;
	}
	UIWidget* pWidget = GetSelected();
	if (pWidget && pWidget->IsInteractable())
	{
		pWidget->SetState(UIWidgetState::NotSelected);
		pWidget->OnDeselected();
	}
	pWidget = m_uUIWidgets->NextSelectableHorizontal(true);
	if (pWidget)
	{
		pWidget->SetState(UIWidgetState::Selected);
		pWidget->OnSelected();
	}
}

void UIContext::OnEnterPressed()
{
	UIWidget* pSelected = GetSelected();
	if (pSelected && pSelected->IsInteractable())
	{
		pSelected->SetState(UIWidgetState::Interacting);
		pSelected->OnInteractStart();
		m_bInteracting = true;
	}
}

void UIContext::OnEnterReleased(bool bInteract)
{
	if (m_bInteracting)
	{
		UIWidget* pSelected = GetSelected();
		if (pSelected && pSelected->IsInteractable())
		{
			pSelected->SetState(UIWidgetState::Selected);
			pSelected->OnInteractEnd(bInteract);
		}
		m_bInteracting = false;
	}
}

void UIContext::OnBackReleased()
{
	if (m_bInteracting)
	{
		OnEnterReleased(false);
	}
	else
	{
		m_onCancelledDelegate();
		m_bDestroyed = m_bAutoDestroyOnCancel;
	}
}

void UIContext::Discard()
{
}
} // namespace LittleEngine
