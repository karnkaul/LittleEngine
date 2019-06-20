#include "stdafx.h"
#include "Core/Logger.h"
#include "SFMLAPI/Rendering/Primitives/SFText.h"
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
	m_pRoot->SetTextColour(m_style.selected.textColour);
}

void UIButton::OnDeselected()
{
	m_pRoot->SetPanel(m_style.notSelected.fill, m_style.notSelected.border, m_style.notSelected.outline);
	m_pRoot->SetTextColour(m_style.notSelected.textColour);
}

void UIButton::OnInteractStart()
{
	m_pRoot->SetPanel(m_style.interacting.fill);
}

void UIButton::OnInteractEnd(bool bInteract)
{
	m_pRoot->SetPanel(m_style.selected.fill, m_style.selected.border, m_style.selected.outline);
	m_pRoot->SetTextColour(m_style.selected.textColour);
	if (bInteract)
	{
		m_OnInteracted();
	}
}

void UIButton::OnSetInteractable(bool bInteractable)
{
	const UIStyle& style = bInteractable ? m_style.notSelected : m_style.uninteractable;
	m_pRoot->SetPanel(style.fill, style.border, style.outline);
	m_pRoot->SetTextColour(style.textColour);
}

void UIButton::Tick(Time dt)
{
	UIWidget::Tick(dt);

	m_pRoot->GetText()->SetPivot({0, -Fixed::OneHalf});
}
} // namespace LittleEngine
