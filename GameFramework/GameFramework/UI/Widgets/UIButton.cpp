#include "stdafx.h"
#include "Logger.h"
#include "LittleEngine/UI/UIElement.h"
#include "LittleEngine/UI/UIText.h"
#include "UIButton.h"

namespace LittleEngine
{
UIButton::UIButton() : UIWidget("Untitled")
{
	SetName("", "UIButton");
}

UIButton::UIButton(String name) : UIWidget(std::move(name))
{
	SetName("", "UIButton");
}

UIButton::~UIButton() = default;

void UIButton::SetText(UIText uiText)
{
	m_pRoot->SetText(std::move(uiText));
}

UIButton::OnClick::Token UIButton::AddCallback(UIButton::OnClick::Callback callback)
{
	return m_OnInteracted.Register(std::move(callback));
}

UIElement* UIButton::GetButtonElement() const
{
	return m_pRoot;
}

void UIButton::OnInitWidget()
{
	m_pRoot = AddElement<UIElement>(String(GetNameStr()) + " Panel");
	m_pRoot->m_transform.size = m_style.widgetSize;
}

void UIButton::OnSelected()
{
	if (m_pRoot)
	{
		m_pRoot->SetPanel(m_style.selected.fill, m_style.selected.border, m_style.selected.outline);
	}
}

void UIButton::OnDeselected()
{
	if (m_pRoot)
	{
		m_pRoot->SetPanel(m_style.notSelected.fill, m_style.notSelected.border, m_style.notSelected.outline);
	}
}

void UIButton::OnInteractStart()
{
	if (m_pRoot)
	{
		m_pRoot->SetPanel(m_style.interacting.fill);
	}
}

void UIButton::OnInteractEnd(bool bInteract)
{
	if (m_pRoot)
	{
		if (m_prevState == UIWidgetState::Selected)
			m_pRoot->SetPanel(m_style.selected.fill, m_style.selected.border, m_style.selected.outline);
		else
			m_pRoot->SetPanel(m_style.notSelected.fill, m_style.notSelected.border,
							  m_style.notSelected.outline);
	}
	if (bInteract)
		m_OnInteracted();
}

void UIButton::SetInteractable(bool bInteractable)
{
	if (m_pRoot)
	{
		UIStyle& style = bInteractable ? m_style.notSelected : m_style.uninteractable;
		m_pRoot->SetPanel(style.fill, style.border, style.outline);
		SetState(bInteractable ? UIWidgetState::NotSelected : UIWidgetState::Uninteractable);
	}
}
} // namespace LittleEngine
