#include "stdafx.h"
#include "Core/Logger.h"
#include "LEGame/Model/UI/UIElement.h"
#include "UIButton.h"

namespace LittleEngine
{
void UIButton::SetText(UIText uiText)
{
	m_pRoot->SetText(std::move(uiText));
}

UIButton::OnClick::Token UIButton::AddCallback(UIButton::OnClick::Callback callback)
{
	return m_OnInteracted.Register(std::move(callback));
}

void UIButton::OnCreated()
{
	SetType("UIButton");
}

void UIButton::OnSelected()
{
	m_pRoot->SetPanel(m_style.selected.fill, m_style.selected.border, m_style.selected.outline);
}

void UIButton::OnDeselected()
{
	m_pRoot->SetPanel(m_style.notSelected.fill, m_style.notSelected.border, m_style.notSelected.outline);
}

void UIButton::OnInteractStart()
{
	m_pRoot->SetPanel(m_style.interacting.fill);
}

void UIButton::OnInteractEnd(bool bInteract)
{
	m_pRoot->SetPanel(m_style.selected.fill, m_style.selected.border, m_style.selected.outline);
	if (bInteract)
	{
		m_OnInteracted();
	}
}

void UIButton::OnSetInteractable(bool bInteractable)
{
	const UIStyle& style = bInteractable ? m_style.notSelected : m_style.uninteractable;
	m_pRoot->SetPanel(style.fill, style.border, style.outline);
}
} // namespace LittleEngine
