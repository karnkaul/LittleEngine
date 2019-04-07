#include "stdafx.h"
#include "Logger.h"
#include "LittleEngine/UI/UIContext.h"
#include "LittleEngine/UI/UIElement.h"
#include "LittleEngine/UI/UIText.h"
#include "UIToggle.h"

namespace LittleEngine
{
UIToggle::UIToggle() : UIWidget("Untitled")
{
	SetName("", "UIToggle");
}

UIToggle::UIToggle(String name) : UIWidget(std::move(name))
{
	SetName("", "UIToggle");
}

UIToggle::~UIToggle()
{
	LOG_D("%s destroyed", LogNameStr());
}

UIToggle* UIToggle::SetText(const UIText& text)
{
	m_pLabel->SetText(text);
	return this;
}

UIToggle* UIToggle::SetOnColour(Colour onColour)
{
	m_data.onColour = onColour;
	return this;
}

UIToggle* UIToggle::SetOffColour(Colour offColour)
{
	m_data.offColour = offColour;
	return this;
}

UIToggle* UIToggle::SetBoxSize(Vector2 size)
{
	m_data.boxSize = size;
	m_pToggle->m_transform.size = m_data.boxSize;
	return this;
}

UIToggle* UIToggle::SetOn(bool bOn)
{
	m_bOn = bOn;
	Colour fill = m_bOn ? m_data.onColour : m_data.offColour;
	UIStyle& style = m_state == UIWidgetState::Selected ? m_style.selected : m_style.notSelected;
	m_pToggle->SetPanel(fill, style.border, style.outline);
	return this;
}

UIToggle::OnChanged::Token UIToggle::AddCallback(const UIToggle::OnChanged::Callback& callback)
{
	return m_delegate.Register(callback);
}

UIElement* UIToggle::GetRoot() const
{
	return m_pRoot;
}

void UIToggle::OnInitWidget()
{
	m_pRoot = AddElement<UIElement>("ToggleRoot");
	m_pRoot->SetPanel(m_style.background);
	m_pRoot->m_transform.size = m_style.widgetSize;
	Colour initColour = m_bOn ? m_data.onColour : m_data.offColour;
	m_pToggle = AddElement<UIElement>("ToggleBox", &m_pRoot->m_transform);
	m_pToggle->SetPanel(initColour, m_style.notSelected.border, m_style.notSelected.outline);
	m_pToggle->m_transform.size = m_data.boxSize;
	m_pToggle->m_transform.anchor = {-1, 0};
	m_pToggle->m_transform.nPosition = {Fixed(-0.9f), 0};
	m_pLabel = AddElement<UIElement>("ToggleLabel", &m_pRoot->m_transform);
	m_pLabel->m_transform.anchor = {1, 0};
	m_pLabel->m_transform.nPosition = {Fixed(0.9f), 0};
}

void UIToggle::OnSelected()
{
	Colour fill = m_bOn ? m_data.onColour : m_data.offColour;
	m_pToggle->SetPanel(fill, m_style.selected.border, m_style.selected.outline);
}

void UIToggle::OnDeselected()
{
	Colour fill = m_bOn ? m_data.onColour : m_data.offColour;
	m_pToggle->SetPanel(fill, m_style.notSelected.border, m_style.notSelected.outline);
}

void UIToggle::OnInteractStart()
{
	m_pToggle->SetPanel(m_style.interacting.fill, m_style.interacting.border, m_style.interacting.outline);
}

void UIToggle::OnInteractEnd(bool bInteract)
{
	if (bInteract)
	{
		m_bOn = !m_bOn;
	}
	Colour fill = m_bOn ? m_data.onColour : m_data.offColour;
	m_pToggle->SetPanel(fill, m_style.selected.border, m_style.selected.outline);
	if (bInteract)
	{
		m_delegate(m_bOn);
	}
}

void UIToggle::SetInteractable(bool bInteractable)
{
	UIStyle& style = bInteractable
						 ? (m_prevState == UIWidgetState::Selected ? m_style.selected : m_style.notSelected)
						 : m_style.uninteractable;
	m_pToggle->SetPanel(style.fill, style.border, style.outline);
	m_state = bInteractable
				  ? (m_prevState == UIWidgetState::Selected ? UIWidgetState::Selected : UIWidgetState::NotSelected)
				  : UIWidgetState::Uninteractable;
}

void UIToggle::Tick(Time dt)
{
	UIWidget::Tick(dt);

	m_pRoot->SetPanel(m_style.background);
	m_pRoot->m_transform.size = m_style.widgetSize;
}
} // namespace LittleEngine
