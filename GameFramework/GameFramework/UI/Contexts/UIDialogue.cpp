#include "stdafx.h"
#include "Logger.h"
#include "LittleEngine/UI/UIElement.h"
#include "LittleEngine/UI/UIWidgetMatrix.h"
#include "UIDialogue.h"

namespace LittleEngine
{
namespace
{
const Fixed headerHeight = 80;
const Fixed footerHeight = 80;
} // namespace

void UIDialogue::OnCreated()
{
	SetName("", "UIDialogue");
	const Fixed contentHeight = m_data.size.y - (headerHeight + footerHeight);
	m_bAutoDestroyOnCancel = true;
	m_pRootElement->m_transform.size = {m_data.size.x, m_data.size.y};
	m_pRootElement->SetPanel(Colour::White);

	m_pHeader = AddElement<UIElement>(String(GetNameStr()) + " Header");
	m_pHeader->m_transform.size = {m_data.size.x, headerHeight};
	m_pHeader->m_transform.bAutoPad = true;
	m_pHeader->m_transform.nPosition = {0, 1};
	m_pHeader->SetPanel(m_data.headerBG);

	m_pContent = AddElement<UIElement>(String(GetNameStr()) + " Content");
	m_pContent->m_transform.size = {m_data.size.x, contentHeight};
	m_pContent->m_transform.nPosition = {0, 1};
	m_pContent->m_transform.padding = {0, -(headerHeight + contentHeight * Fixed::OneHalf)};
	m_pContent->SetPanel(m_data.contentBG);
}

UIDialogue* UIDialogue::SetContent(UIText text, const Colour* pBackground, const Vector2* pSize)
{
	if (pSize)
	{
		m_data.size = *pSize;
		const Fixed contentHeight = m_data.size.y - (headerHeight + footerHeight);
		m_pContent->m_transform.size = {m_data.size.x, contentHeight};
		m_pHeader->m_transform.size = {m_data.size.x, headerHeight};
		if (m_pFooter)
			m_pFooter->m_transform.size = {m_data.size.x, footerHeight};
	}

	if (pBackground)
	{
		m_data.contentBG = *pBackground;
		m_pContent->SetPanel(m_data.contentBG);
	}

	m_pContent->SetText(std::move(text));
	return this;
}

UIDialogue* UIDialogue::SetHeader(UIText text, const Colour* pBackground)
{
	if (pBackground)
	{
		m_data.headerBG = *pBackground;
		m_pHeader->SetPanel(m_data.headerBG);
	}

	m_pHeader->SetText(std::move(text));
	return this;
}

UIButton::OnClick::Token UIDialogue::AddMainButton(UIText text,
												   UIButton::OnClick::Callback onMainButton,
												   bool bDismissOnBack)
{
	if (m_pMainButton)
	{
		LOG_W("%s Main button already exists on this UIDialogue! Ignoring call to AddMainButton", LogNameStr());
		return nullptr;
	}

	m_pFooter = AddElement<UIElement>(String(GetNameStr()) + " Footer");
	m_pFooter->m_transform.size = {m_data.size.x, footerHeight};
	m_pFooter->m_transform.bAutoPad = true;
	m_pFooter->m_transform.nPosition = {0, -1};

	m_pMainButton = AddWidget<UIButton>(String(GetNameStr()) + " Button 0");
	m_pMainButton->SetText(std::move(text));
	UIElement* pButtonEl = m_pMainButton->GetButtonElement();
	pButtonEl->m_transform.SetParent(m_pFooter->m_transform);
	m_bAutoDestroyOnCancel = bDismissOnBack;
	return m_pMainButton->AddCallback(std::move(onMainButton));
}

UIButton::OnClick::Token UIDialogue::AddOtherButton(UIText otherButtonUIText,
													UIButton::OnClick::Callback onOtherButton,
													bool bSelect)
{
	if (m_pOtherButton)
	{
		LOG_W("%s Other button already exists on this UIDialogue! Ignoring call to AddOtherButton",
			  LogNameStr());
		return nullptr;
	}
	m_pOtherButton = AddWidget<UIButton>("Dialog Button 0", nullptr, true);
	m_pOtherButton->SetText(std::move(otherButtonUIText));
	UIElement* pOtherButtonEl = m_pOtherButton->GetButtonElement();
	pOtherButtonEl->m_transform.SetParent(m_pFooter->m_transform);
	pOtherButtonEl->m_transform.nPosition = {Fixed::OneHalf, 0};

	UIElement* pMainButtonEl = m_pMainButton->GetButtonElement();
	pMainButtonEl->m_transform.SetParent(m_pFooter->m_transform);
	pMainButtonEl->m_transform.nPosition = {-Fixed::OneHalf, 0};

	// Terrible hack
	if (!bSelect)
		OnLeft();

	return m_pOtherButton->AddCallback(std::move(onOtherButton));
}
} // namespace LittleEngine
