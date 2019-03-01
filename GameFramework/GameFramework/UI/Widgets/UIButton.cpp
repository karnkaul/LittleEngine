#include "stdafx.h"
#include "UIButton.h"
#include "Logger.h"

namespace LittleEngine
{
UIButton::UIButton() : UIWidget("Untitled")
{
	SetName("", "UIButton");
}

UIButton::UIButton(const String& name) : UIWidget(name)
{
	SetName("", "UIButton");
}

UIButton::~UIButton()
{
	LOG_D("%s destroyed", LogNameStr());
}

void UIButton::SetText(const UIText& uiText)
{
	m_pRoot->SetText(uiText);
}

UIButton::OnClick::Token UIButton::AddCallback(const UIButton::OnClick::Callback& Callback)
{
	return m_OnInteracted.Register(Callback);
}

UIElement* UIButton::GetButtonElement() const
{
	return m_pRoot;
}

void UIButton::OnInitWidget()
{
	m_pRoot = AddElement<UIElement>(String(GetNameStr()) + " Panel");
	m_pRoot->m_transform.size = m_style.widgetSize;
	m_pRoot->SetPanel(m_style.notSelected.fill, m_style.notSelected.border, m_style.notSelected.outline);
}

void UIButton::OnSelected()
{
	m_bSelected = true;
	if (m_pRoot)
	{
		m_pRoot->SetPanel(m_style.selected.fill, m_style.selected.border, m_style.selected.outline);
	}
}

void UIButton::OnDeselected()
{
	m_bSelected = false;
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

void UIButton::OnInteractEnd()
{
	if (m_pRoot)
	{
		if (m_bSelected)
			m_pRoot->SetPanel(m_style.selected.fill, m_style.selected.border, m_style.selected.outline);
		else
			m_pRoot->SetPanel(m_style.notSelected.fill, m_style.notSelected.border, m_style.notSelected.outline);
	}
	m_OnInteracted();
}
} // namespace LittleEngine
