#include "stdafx.h"
#include "UIToggle.h"
#include "LittleEngine/UI/UIContext.h"

namespace LittleEngine
{
UIToggle::UIToggle() : UIWidget("Untitled")
{
	SetName("", "UIToggle");
}

UIToggle::UIToggle(const String& name) : UIWidget(name)
{
	SetName("", "UIToggle");
}

UIToggle::~UIToggle()
{
	LOG_D("%s destroyed", LogNameStr());
}

OnChanged::Token UIToggle::InitToggle(const UIToggleData& data, OnChanged::Callback onChanged)
{
	m_data = data;
	m_bOn = m_data.bSetOn;
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
	m_pLabel->SetText(m_data.label);
	m_pLabel->m_transform.anchor = {1, 0};
	m_pLabel->m_transform.nPosition = {Fixed(0.9f), 0};
	return m_delegate.Register(onChanged);
}

void UIToggle::SetText(const UIText& text)
{
	m_pLabel->SetText(text);
}

OnChanged::Token UIToggle::AddCallback(OnChanged::Callback callback)
{
	return m_delegate.Register(callback);
}

void UIToggle::SetOn(bool bOn)
{
	m_bOn = bOn;
	Colour fill = m_bOn ? m_data.onColour : m_data.offColour;
	UIStyle& style = m_state == UIWidgetState::Selected ? m_style.selected : m_style.notSelected;
	m_pToggle->SetPanel(fill, style.border, style.outline);
}

UIElement* UIToggle::GetRoot() const
{
	return m_pRoot;
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

void UIToggle::OnInteractEnd()
{
	m_bOn = !m_bOn;
	Colour fill = m_bOn ? m_data.onColour : m_data.offColour;
	m_pToggle->SetPanel(fill, m_style.selected.border, m_style.selected.outline);
	m_delegate(m_bOn);
}
} // namespace LittleEngine
