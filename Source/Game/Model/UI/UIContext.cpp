#include "Core/LECoreGame/LECoreUtils/Logger.h"
#include "SFMLAPI/Rendering/Primitives/SFRect.h"
#include "Engine/Context/LEContext.h"
#include "Engine/GFX.h"
#include "UIContext.h"
#include "UIWidget.h"
#include "Model/GameManager.h"
#include "Model/World/World.h"
#include "Model/UI/UIElement.h"

namespace LittleEngine
{
UIContext::UIContext() = default;

UIContext::~UIContext()
{
	SetActive(false);
	m_uUIWidgets->Clear();
	m_uiElements.clear();
}

void UIContext::OnCreate(std::string id, LayerID rootLayer)
{
	SetNameAndType(std::move(id), "UIContext");
	m_uUIWidgets = std::make_unique<UIWidgetMatrix>();
	s32 layerDelta = ToS32(rootLayer) - ToS32(LayerID::UI);
	m_pRoot = AddElement<UIElement>(m_name + "_Root", nullptr, layerDelta);
	m_pRoot->SetStatic(true);
	Assert(g_pGameManager, "GameManager is null!");
	OnCreated();
}

void UIContext::SetEnabled(bool bSetEnabled)
{
	m_bEnabled = bSetEnabled;
	m_ioToken = nullptr;
	if (bSetEnabled)
	{
		m_bDestroyed = false;
		m_ioToken = g_pGameManager->Input()->Register([&](const auto& frame) -> bool { return OnInput(frame); }, true);
	}
	for (auto& uElement : m_uiElements)
	{
		uElement->SetEnabled(bSetEnabled);
	}
	m_uUIWidgets->ForEach([bSetEnabled](UPtr<UIWidget>& uWidget) {
		for (auto& uElement : uWidget->m_uiElements)
		{
			uElement->SetEnabled(bSetEnabled);
		}
	});
	OnEnabling(bSetEnabled);
}

LayerID UIContext::MaxLayer() const
{
	LayerID maxLayer = m_pRoot->m_layer;
	for (const auto& uElement : m_uiElements)
	{
		if (uElement->m_layer > maxLayer)
		{
			maxLayer = uElement->m_layer;
		}
	}
	m_uUIWidgets->ForEach([&maxLayer](const UPtr<UIWidget>& uWidget) {
		if (uWidget->MaxLayer() > maxLayer)
		{
			maxLayer = uWidget->MaxLayer();
		}
	});
	return maxLayer;
}

void UIContext::SetActive(bool bActive, bool bResetSelection)
{
	m_bActive = bActive;
	if (m_bActive)
	{
		Assert(g_pGameManager, "GameManager is null!");
		m_ptrToken = g_pGameManager->Context()->PushPointer(LEContext::Pointer::Type::Arrow);
		if (!m_bEnabled)
		{
			SetEnabled(true);
		}
		if (bResetSelection)
		{
			ResetSelection();
		}
		OnActivated();
		Tick();
	}
}

void UIContext::ResetSelection()
{
	m_uUIWidgets->Reset(true);
	m_uUIWidgets->ForEach([](UPtr<UIWidget>& uWidget) { uWidget->Deselect(); });
	UIWidget* pSelected = Selected();
	if (pSelected)
	{
		pSelected->Select();
	}
	Tick();
}

UIWidget* UIContext::Selected()
{
	return m_uUIWidgets->Current();
}

UIElement* UIContext::Root() const
{
	return m_pRoot;
}

Token UIContext::SetOnCancelled(OnCancelled::Callback callback, bool bAutoDestroy)
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
	if (m_bActive)
	{
		bool bPointerSelection = false;
		const MouseInput& pointerState = g_pGameManager->Input()->MouseState();
		if (pointerState.bInViewport)
		{
			UIWidget* pSelected = Selected();
			m_uUIWidgets->ForEach([this, &pSelected, &bPointerSelection, &pointerState](UIContext::UUIWidget& uUIWidget) {
				Vector2 mp = g_pGFX->WorldToUI(pointerState.worldPosition);
				if (!bPointerSelection && uUIWidget->IsPointInBounds(mp))
				{
					bPointerSelection = true;
					auto pToActivate = uUIWidget.get();
					if (pSelected != pToActivate)
					{
						if (m_uUIWidgets->Select(pToActivate))
						{
							if (pSelected)
							{
								pSelected->Deselect();
							}
							pToActivate->Select();
						}
						else
						{
							bPointerSelection = false;
							LOG_E("%s ERROR! %s not found in widget matrix!", m_logName.c_str(), pToActivate->m_logName.c_str());
						}
					}
				}
			});
		}
		if (bPointerSelection)
		{
			// Pointer is inside a widget's bounds
			m_pPointerOver = Selected();
			if (m_pPointerOver)
			{
				if (m_mbState.bEnterPressed)
				{
					OnEnterPressed();
				}
				else if (m_mbState.bEnterReleased)
				{
					OnEnterReleased(m_bInteracting);
				}
			}
		}
		else
		{
			if (m_bInteracting && m_pPointerOver)
			{
				// Pointer has moved out
				OnEnterReleased(false);
			}
			m_pPointerOver = nullptr;
		}
	}
	m_uUIWidgets->ForEach([dt](UIContext::UUIWidget& uUIWidget) { uUIWidget->Tick(dt); });
}

bool UIContext::IsPersistent() const
{
	return m_bPersistent;
}

void UIContext::SetPersistent(bool bPersistent)
{
	m_bPersistent = bPersistent;
}

void UIContext::Regenerate(LayerID newLayer)
{
	for (auto& uElement : m_uiElements)
	{
		uElement->Regenerate(newLayer);
	}
	m_uUIWidgets->ForEach([newLayer](UPtr<UIWidget>& uWidget) {
		for (auto& uElement : uWidget->m_uiElements)
		{
			uElement->Regenerate(newLayer);
		}
	});
	Tick();
}

void UIContext::OnCreated() {}

void UIContext::OnActivated() {}

void UIContext::OnDestroying() {}

void UIContext::OnEnabling(bool /*bEnabled*/) {}

bool UIContext::OnInput(const LEInput::Frame& frame)
{
	if (m_bDestroyed || !m_bActive)
	{
		return false;
	}
	m_mbState.bEnterPressed = frame.IsPressed(KeyType::MOUSE_BTN_0);
	m_mbState.bEnterReleased = frame.IsReleased(KeyType::MOUSE_BTN_0);
	if (frame.IsPressed({KeyCode::Enter, KeyType::JOY_BTN_0}))
	{
		OnEnterPressed();
	}
	if (frame.IsReleased({KeyCode::Enter, KeyType::JOY_BTN_0}))
	{
		OnEnterReleased(m_bInteracting);
	}
	if (frame.IsReleased({KeyCode::Escape, KeyType::JOY_BTN_1, KeyType::MOUSE_BTN_1}))
	{
		OnBackReleased();
	}
	if (frame.IsReleased(KeyCode::Up))
	{
		OnUp();
	}
	if (frame.IsReleased(KeyCode::Down))
	{
		OnDown();
	}
	if (frame.IsReleased(KeyCode::Left))
	{
		OnLeft();
	}
	if (frame.IsReleased(KeyCode::Right))
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
	UIWidget* pSelected = Selected();
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
	UIWidget* pWidget = Selected();
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
	UIWidget* pWidget = Selected();
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
	UIWidget* pWidget = Selected();
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
	UIWidget* pSelected = Selected();
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
		UIWidget* pSelected = Selected();
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
