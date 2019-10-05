#include "Core/Logger.h"
#include "SFMLAPI/Rendering/Primitives/SFRect.h"
#include "SFMLAPI/Rendering/Primitives/SFText.h"
#include "UIWidget.h"
#include "Model/GameManager.h"
#include "Model/UI/UIGameStyle.h"
#include "Model/UI/UITransform.h"
#include "UIContext.h"

namespace LittleEngine
{
UIWidget::UIWidget() = default;
UIWidget::~UIWidget() = default;

UIWidgetStyle& UIWidget::GetStyle()
{
	return m_style;
}

UIElement* UIWidget::Root() const
{
	return m_pRoot;
}

void UIWidget::SetStyle(const UIWidgetStyle& style)
{
	m_style = style;
}

void UIWidget::SetInteractable(bool bInteractable)
{
	SetState(bInteractable ? State::NotSelected : State::Uninteractable);
	OnSetInteractable(bInteractable);
}

bool UIWidget::IsInteractable() const
{
	return m_state != State::Uninteractable;
}

void UIWidget::SetState(State state)
{
	m_prevState = m_state == State::Interacting ? state : m_state;
	m_state = state;
}

bool UIWidget::IsPointInBounds(Vector2 point) const
{
	// Rect2 uiRect = g_pGFX->WorldToUI(Root()->Rect()->GameBounds());
	Rect2 uiRect = Root()->Rect()->GameBounds();
	return uiRect.IsPointIn(point);
}

void UIWidget::OnCreated() {}

void UIWidget::Tick(Time dt)
{
	if (m_bDestroyed)
	{
		return;
	}
	for (auto& uUIelement : m_uiElements)
	{
		uUIelement->Tick(dt);
	}
}

void UIWidget::Select()
{
	if (IsInteractable())
	{
		SetState(State::Selected);
		OnSelected();
	}
}

void UIWidget::Deselect()
{
	if (IsInteractable())
	{
		SetState(State::NotSelected);
		OnDeselected();
	}
}

void UIWidget::InteractStart()
{
	if (IsInteractable())
	{
		SetState(State::Interacting);
		OnInteractStart();
	}
}

void UIWidget::InteractEnd(bool bInteract)
{
	if (IsInteractable())
	{
		SetState(State::Selected);
		OnInteractEnd(bInteract);
	}
}

void UIWidget::OnCreate(std::string name, UIContext& owner, UIWidgetStyle* pStyleToCopy)
{
	SetNameAndType(std::move(name), "UIWidget");
	m_pOwner = &owner;
	m_style = pStyleToCopy ? *pStyleToCopy : UIGameStyle::GetStyle("");
	m_pRoot = AddElement<UIElement>(m_name + "_Root");
	m_pRoot->SetRectSize(m_style.widgetSize);
	OnCreated();
	SetInteractable(true);
}

void UIWidget::InitElement(std::string name, UIElement* pNewElement, UITransform* pParent)
{
	if (!pParent)
	{
		pParent = m_pOwner ? &m_pOwner->Root()->m_transform : nullptr;
	}
	pNewElement->OnCreate(*g_pGameManager->Context(), std::move(name), pParent, m_style.pFont);
}

LayerID UIWidget::MaxLayer() const
{
	LayerID maxLayer = LayerID::Zero;
	for (const auto& uElement : m_uiElements)
	{
		if (uElement->m_layer > maxLayer)
		{
			maxLayer = uElement->m_layer;
		}
	}
	return maxLayer;
}
} // namespace LittleEngine
