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
	m_bOn = data.bSetOn;
	m_pRoot = AddElement<UIElement>("ToggleRoot");
	m_pRoot->SetPanel(m_data.bgColour);
	m_pRoot->m_transform.size = data.totalSize;
	Colour initColour = m_bOn ? m_data.onColour : m_data.offColour;
	m_pToggle = AddElement<UIElement>("ToggleBox", &m_pRoot->m_transform);
	m_pToggle->SetPanel(initColour, m_data.borderSize, m_data.borderColourNS);
	m_pToggle->m_transform.size = m_data.boxSize;
	m_pToggle->m_transform.anchor = {-1, 0};
	m_pToggle->m_transform.nPosition = {Fixed(-0.9f), 0};
	m_pLabel = AddElement<UIElement>("ToggleLabel", &m_pRoot->m_transform);
	m_pLabel->SetText(data.labelText);
	m_pLabel->m_transform.anchor = {1, 0};
	m_pLabel->m_transform.nPosition = {Fixed(0.9f), 0};
	return m_delegate.Register(onChanged);
}

UIElement* UIToggle::GetRoot() const
{
	return m_pRoot;
}

void UIToggle::OnSelected()
{
	Colour fill = m_bOn ? m_data.onColour : m_data.offColour;
	m_pToggle->SetPanel(fill, m_data.borderSize, m_data.borderColourS);
}

void UIToggle::OnDeselected()
{
	Colour fill = m_bOn ? m_data.onColour : m_data.offColour;
	m_pToggle->SetPanel(fill, m_data.borderSize, m_data.borderColourNS);
}

void UIToggle::OnInteractStart()
{
	m_pToggle->SetPanel(m_data.interactColour, m_data.borderSize, m_data.borderColourS);
}

void UIToggle::OnInteractEnd()
{
	m_bOn = !m_bOn;
	Colour fill = m_bOn ? m_data.onColour : m_data.offColour;
	m_pToggle->SetPanel(fill, m_data.borderSize, m_data.borderColourS);
	m_delegate(m_bOn);
}
} // namespace LittleEngine
