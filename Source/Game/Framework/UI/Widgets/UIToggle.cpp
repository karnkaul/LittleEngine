#include "Model/UI/UIContext.h"
#include "Model/UI/UIElement.h"
#include "UIToggle.h"

namespace LittleEngine
{
UIToggle* UIToggle::SetText(UIText text)
{
	m_pLabel->SetText(std::move(text));
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

UIToggle* UIToggle::SetBackground(Colour colour)
{
	m_style.background = colour;
	m_pRoot->SetPanel(m_style.background);
	return this;
}

UIToggle* UIToggle::SetBoxSize(Vector2 size)
{
	m_data.boxSize = size;
	m_pToggle->SetRectSize(m_data.boxSize);
	return this;
}

UIToggle* UIToggle::SetOn(bool bOn)
{
	m_bOn = bOn;
	Colour fill = m_bOn ? m_data.onColour : m_data.offColour;
	UIStyle& style = m_state == State::Selected ? m_style.selected : m_style.notSelected;
	m_pToggle->SetPanel(fill, style.border, style.outline);
	m_pToggle->SetTextColour(style.textColour);
	return this;
}

Token UIToggle::AddCallback(UIToggle::OnChanged::Callback callback)
{
	return m_delegate.Register(std::move(callback));
}

void UIToggle::OnCreated()
{
	SetType("UIToggle");
	m_pRoot->SetPanel(m_style.background);
	m_pRoot->SetRectSize(m_style.widgetSize);
	m_pRoot->SetStatic(true);
	Colour initColour = m_bOn ? m_data.onColour : m_data.offColour;
	m_pToggle = AddElement<UIElement>("ToggleBox", &m_pRoot->m_transform, 2);
	m_pToggle->SetPanel(initColour, m_style.notSelected.border, m_style.notSelected.outline);
	m_pToggle->SetRectSize(m_data.boxSize);
	m_pToggle->m_transform.anchor = {-1, 0};
	m_pToggle->m_transform.nPosition = {Fixed(-0.9f), 0};
	m_pLabel = AddElement<UIElement>("ToggleLabel", &m_pRoot->m_transform, 2);
	m_pLabel->m_transform.anchor = {-1, 0};
	m_pLabel->m_transform.nPosition = {Fixed(0.9f), 0};
	m_pLabel->SetTextColour(m_style.notSelected.textColour);
	Fixed xPad = Maths::Max(m_style.widgetSize.x - m_data.boxSize.x - 50, Fixed::Zero);
	m_pLabel->m_transform.posDelta = {-xPad, 8};
}

void UIToggle::OnSelected()
{
	Colour fill = m_bOn ? m_data.onColour : m_data.offColour;
	m_pToggle->SetPanel(fill, m_style.selected.border, m_style.selected.outline);
	m_pLabel->SetTextColour(m_style.selected.textColour);
}

void UIToggle::OnDeselected()
{
	Colour fill = m_bOn ? m_data.onColour : m_data.offColour;
	m_pToggle->SetPanel(fill, m_style.notSelected.border, m_style.notSelected.outline);
	m_pLabel->SetTextColour(m_style.notSelected.textColour);
}

void UIToggle::OnInteractStart()
{
	m_pToggle->SetPanel(m_style.interacting.fill, m_style.interacting.border, m_style.interacting.outline);
	m_pLabel->SetTextColour(m_style.interacting.textColour);
}

void UIToggle::OnInteractEnd(bool bInteract)
{
	if (bInteract)
	{
		m_bOn = !m_bOn;
	}
	Colour fill = m_bOn ? m_data.onColour : m_data.offColour;
	m_pToggle->SetPanel(fill, m_style.selected.border, m_style.selected.outline);
	m_pLabel->SetTextColour(m_style.selected.textColour);
	if (bInteract)
	{
		m_delegate(m_bOn);
	}
}

void UIToggle::OnSetInteractable(bool bInteractable)
{
	const UIStyle& style = bInteractable ? m_style.notSelected : m_style.uninteractable;
	m_pToggle->SetPanel(style.fill, style.border, style.outline);
	m_pLabel->SetTextColour(style.textColour);
}
} // namespace LittleEngine
