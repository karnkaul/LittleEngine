#include "stdafx.h"
#include "UITextInput.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/Engine/EngineService.h"

namespace LittleEngine
{
UITextInput::UITextInput() : UIWidget("Untitled")
{
	SetName("", "UITextInput");
}

UITextInput::UITextInput(const String& name) : UIWidget(name)
{
	SetName("", "UITextInput");
}

UITextInput::~UITextInput()
{
	LOG_D("%s destroyed", LogNameStr());
}

UITextInput* UITextInput::SetTextColour(Colour text)
{
	m_data.text = text;
	return this;
}

UITextInput* UITextInput::SetTextStyle(const UIText& uiText)
{
	m_data.textStyle = uiText;
	return this;
}

UITextInput::OnEditComplete::Token UITextInput::SetOnEditComplete(const OnEditComplete::Callback& callback)
{
	return m_onEditComplete.Register(callback);
}

void UITextInput::SetInteractable(bool bInteractable)
{
	UIStyle& style = bInteractable
						 ? (m_prevState == UIWidgetState::Selected ? m_style.selected : m_style.notSelected)
						 : m_style.uninteractable;
	m_pRoot->SetPanel(style.fill, style.border, style.outline);
	m_state = bInteractable
				  ? (m_prevState == UIWidgetState::Selected ? UIWidgetState::Selected : UIWidgetState::NotSelected)
				  : UIWidgetState::Uninteractable;
}

void UITextInput::OnInitWidget()
{
	m_keyboard.m_bClearOnEscape = false;
	m_style = UIWidgetStyle::GetDefault1(&m_style);
	
	m_pRoot = AddElement<UIElement>("TextInputRoot");
	m_pRoot->SetPanel(m_style.background);
	m_pRoot->m_transform.size = m_style.widgetSize;

	m_pText = AddElement<UIElement>("TextInputText");
	m_pText->m_transform.SetParent(m_pRoot->m_transform);
	m_pText->m_transform.anchor = {-1, 1};
	m_pText->m_transform.nPosition = {-Fixed(0.95f), Fixed(0.4f)};
	m_pText->GetText()->SetPrimaryColour(m_data.text);
}

void UITextInput::OnSelected()
{
	m_pRoot->SetPanel(m_style.selected.fill, m_style.selected.border, m_style.selected.outline);
}

void UITextInput::OnDeselected()
{
	m_token = nullptr;
	m_pRoot->SetPanel(m_style.background);
}

void UITextInput::OnInteractStart()
{
	m_pRoot->SetPanel(m_style.interacting.fill, m_style.interacting.border, m_style.interacting.outline);
}

void UITextInput::OnInteractEnd(bool bInteract)
{
	if (bInteract)
	{
		m_bWriting = !m_bWriting;
		m_token = m_bWriting
					  ? Services::Engine()->Input()->Register(std::bind(&UITextInput::OnInput, this, _1))
					  : nullptr;
	}
	Colour fill = m_bWriting ? m_style.interacting.fill : m_style.selected.fill;
	m_pRoot->SetPanel(fill, m_style.selected.border, m_style.selected.outline);
	if (!m_bWriting)
	{
		m_pText->SetText(
			UIText(m_keyboard.GetLiveString(), m_data.textStyle.pixelSize, m_data.textStyle.colour));
		m_onEditComplete(m_keyboard.GetLiveString());
	}
}

void UITextInput::Tick(Time dt)
{
	if (m_bWriting)
	{
		m_elapsed += dt;
		if (m_elapsed >= m_cursorFreq)
		{
			m_elapsed = Time::Zero;
			m_bShowCursor = !m_bShowCursor;
		}
		String suffix = m_bShowCursor ? "_" : "";
		m_pText->SetText(UIText(m_keyboard.GetLiveString() + suffix, m_data.textStyle.pixelSize,
								m_data.textStyle.colour));
	}

	UIWidget::Tick(dt);
}

bool UITextInput::OnInput(const EngineInput::Frame& frame)
{
	if (m_bWriting)
	{
		if (frame.IsReleased(GameInputType::Enter))
		{
			m_prevText = m_keyboard.GetLiveString();
			OnInteractEnd(true);
			return true;
		}
		if (frame.IsReleased(GameInputType::Back))
		{
			m_keyboard.m_liveLine.liveString = m_prevText;
			OnInteractEnd(true);
			return true;
		}
	}
	m_keyboard.Update(frame);
	return true;
}
} // namespace LittleEngine