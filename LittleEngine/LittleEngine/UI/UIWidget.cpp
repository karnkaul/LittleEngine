#include "stdafx.h"
#include "Core/Logger.h"
#include "UIWidget.h"
#include "LittleEngine/UI/UIElement.h"
#include "LittleEngine/UI/UIContext.h"
#include "LittleEngine/UI/UITransform.h"

namespace LittleEngine
{
UIWidget::UIWidget() = default;
UIWidget::~UIWidget() = default;

void UIWidget::OnCreate(String name, UIContext& owner, UIWidgetStyle* pStyleToCopy)
{
	SetNameAndType(std::move(name), "UIWidget");
	m_pOwner = &owner;
	m_style = pStyleToCopy ? *pStyleToCopy : UIWidgetStyle::GetDefault0();
	OnCreated();
	SetInteractable(true);
}

UIWidgetStyle& UIWidget::GetStyle()
{
	return m_style;
}

void UIWidget::SetStyle(const UIWidgetStyle& style)
{
	m_style = style;
}

bool UIWidget::IsInteractable() const
{
	return m_state != UIWidgetState::Uninteractable;
}

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

void UIWidget::SetState(UIWidgetState state)
{
	m_prevState = m_state == UIWidgetState::Interacting ? state : m_state;
	m_state = state;
}

void UIWidget::InitElement(String name, UIElement* pNewElement, UITransform* pParent)
{
	if (!pParent)
	{
		pParent = m_pOwner ? &m_pOwner->GetRootElement()->m_transform : nullptr;
	}
	pNewElement->OnCreate(std::move(name), pParent);
	pNewElement->m_layer = m_style.baseLayer;
}

LayerID UIWidget::GetMaxLayer() const
{
	LayerID maxLayer = LAYER_ZERO;
	for (const auto& uElement : m_uiElements)
	{
		if (uElement->m_layer > maxLayer)
		{
			maxLayer = uElement->m_layer;
		}
	}
	return maxLayer;
}

void UIWidget::OnCreated()
{
}
} // namespace LittleEngine
