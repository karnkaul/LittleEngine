#include "stdafx.h"
#include "Core/Logger.h"
#include "UIContext.h"
#include "LittleEngine/Engine/EngineService.h"
#include "LittleEngine/Game/World/World.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/UI/UIElement.h"
#include "LittleEngine/UI/UIWidget.h"
#include "LittleEngine/UI/UIWidgetMatrix.h"

namespace LittleEngine
{
UIContext::UIContext() = default;

UIContext::~UIContext()
{
	SetActive(false);
	m_uUIWidgets->Clear();
	m_uiElements.clear();
}

void UIContext::OnCreate(String name, LayerID rootLayer)
{
	SetNameAndType(std::move(name), "UIContext");
	m_uUIWidgets = MakeUnique<UIWidgetMatrix>();
	s32 layerDelta = rootLayer - LAYER_UI;
	m_pRootElement = AddElement<UIElement>(String(GetNameStr()) + "_Root", nullptr, layerDelta);
	OnCreated();
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
	m_uUIWidgets->ForEach([&maxLayer](const UPtr<UIWidget>& uWidget) {
		if (uWidget->GetMaxLayer() > maxLayer)
		{
			maxLayer = uWidget->GetMaxLayer();
		}
	});
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
		m_inputTokens.push_back(Services::Engine()->Input()->Register(
			[&](const EngineInput::Frame& frame) -> bool { return OnInput(frame); }));
	}
}

void UIContext::ResetSelection()
{
	m_uUIWidgets->Reset(true);
	m_uUIWidgets->ForEach([](UPtr<UIWidget>& uWidget) { uWidget->Deselect(); });
	UIWidget* pSelected = GetSelected();
	if (pSelected)
	{
		pSelected->Select();
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

UIContext::OnCancelled::Token UIContext::SetOnCancelled(OnCancelled::Callback callback, bool bAutoDestroy)
{
	m_bAutoDestroyOnCancel = bAutoDestroy;
	return m_onCancelledDelegate.Register(std::move(callback));
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

void UIContext::OnCreated()
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
	if (pSelected)
	{
		pSelected->Deselect();
	}
	pSelected = m_uUIWidgets->NextSelectableVertical(false);
	if (pSelected)
	{
		pSelected->Select();
	}
}

void UIContext::OnDown()
{
	if (m_bInteracting || m_uUIWidgets->CurrentVecCount() < 2)
	{
		return;
	}
	UIWidget* pWidget = GetSelected();
	if (pWidget)
	{
		pWidget->Deselect();
	}
	pWidget = m_uUIWidgets->NextSelectableVertical(true);
	if (pWidget)
	{
		pWidget->Select();
	}
}

void UIContext::OnLeft()
{
	if (m_bInteracting || m_uUIWidgets->NumColumns() < 2)
	{
		return;
	}
	UIWidget* pWidget = GetSelected();
	if (pWidget)
	{
		pWidget->Deselect();
	}
	pWidget = m_uUIWidgets->NextSelectableHorizontal(false);
	if (pWidget)
	{
		pWidget->Select();
	}
}

void UIContext::OnRight()
{
	if (m_bInteracting || m_uUIWidgets->NumColumns() < 2)
	{
		return;
	}
	UIWidget* pWidget = GetSelected();
	if (pWidget)
	{
		pWidget->Deselect();
	}
	pWidget = m_uUIWidgets->NextSelectableHorizontal(true);
	if (pWidget)
	{
		pWidget->Select();
	}
}

void UIContext::OnEnterPressed()
{
	UIWidget* pSelected = GetSelected();
	if (pSelected && pSelected->IsInteractable())
	{
		pSelected->InteractStart();
		m_bInteracting = true;
	}
}

void UIContext::OnEnterReleased(bool bInteract)
{
	if (m_bInteracting)
	{
		UIWidget* pSelected = GetSelected();
		if (pSelected)
		{
			pSelected->InteractEnd(bInteract);
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
} // namespace LittleEngine
