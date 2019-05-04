#include "stdafx.h"
#include "Logger.h"
#include "LittleEngine/UI/UIElement.h"
#include "LittleEngine/UI/UIWidgetMatrix.h"
#include "UIButtonDrawer.h"

namespace LittleEngine
{
UIButtonDrawer::UIButtonDrawerData::UIButtonDrawerData()
{
	panelStyle.size = {600, 500};
}

UIButtonDrawer::UIButtonDrawer() = default;
UIButtonDrawer::~UIButtonDrawer() = default;

bool UIButtonDrawer::SetHorizontal(bool bHorizontal)
{
	if (m_uiButtons.empty())
	{
		m_data.bHorizontal = bHorizontal;
		return true;
	}
	LOG_W("[UIButtonDrawer] Cannot change orientation after adding Buttons!");
	return false;
}

UIButtonDrawer* UIButtonDrawer::SetPanel(UIStyle panelStyle)
{
	m_data.panelStyle = std::move(panelStyle);
	m_pRootElement->m_transform.size = m_data.panelStyle.size;
	m_pRootElement->SetPanel(m_data.panelStyle.fill, m_data.panelStyle.border, m_data.panelStyle.outline);
	return this;
}

UIButton::OnClick::Token UIButtonDrawer::AddButton(UIText buttonText,
												   UIButton::OnClick::Callback onInteracted,
												   UIButton** ppButton)
{
	String buttonName = "Button" + Strings::ToString(m_uiButtons.size());
	UIButton* pButton = AddWidget<UIButton>(buttonName, nullptr, m_data.bHorizontal);
	pButton->SetText(std::move(buttonText));
	m_uiButtons.push_back(pButton);
	SetButtonPositions();
	if (ppButton)
		*ppButton = pButton;
	return pButton->AddCallback(std::move(onInteracted));
}

void UIButtonDrawer::OnCreated()
{
	SetType("UIButtonDrawer");
	m_pRootElement->m_transform.size = m_data.panelStyle.size;
	m_pRootElement->SetPanel(m_data.panelStyle.fill, m_data.panelStyle.border, m_data.panelStyle.outline);
}

void UIButtonDrawer::SetButtonPositions()
{
	u32 count = static_cast<u32>(m_uiButtons.size());
	// Special cases
	if (count == 1)
	{
		m_uiButtons[0]->GetButtonElement()->m_transform.nPosition = {0, 0};
		return;
	}
	if (count == 2)
	{
		Fixed midPoint = (m_data.btnNPosRange.max + m_data.btnNPosRange.min) * Fixed::OneHalf;
		Fixed upperMid = (m_data.btnNPosRange.max + midPoint) * Fixed::OneHalf;
		Fixed lowerMid = (m_data.btnNPosRange.min + midPoint) * Fixed::OneHalf;
		Vector2 upper = m_data.bHorizontal ? Vector2(lowerMid, 0) : Vector2(0, upperMid);
		Vector2 lower = m_data.bHorizontal ? Vector2(upperMid, 0) : Vector2(0, lowerMid);
		m_uiButtons[0]->GetButtonElement()->m_transform.nPosition = upper;
		m_uiButtons[1]->GetButtonElement()->m_transform.nPosition = lower;
		return;
	}

	// [+Y -> -Y] or [-X -> +X]
	Fixed step = Fixed(m_data.btnNPosRange.max - m_data.btnNPosRange.min) / (count - 1);
	Fixed current = m_data.bHorizontal ? m_data.btnNPosRange.min : m_data.btnNPosRange.max;
	for (auto pButton : m_uiButtons)
	{
		Vector2 pos = m_data.bHorizontal ? Vector2(current, 0) : Vector2(0, current);
		pButton->GetButtonElement()->m_transform.nPosition = pos;
		current += (m_data.bHorizontal ? step : -step);
	}
}
} // namespace LittleEngine
