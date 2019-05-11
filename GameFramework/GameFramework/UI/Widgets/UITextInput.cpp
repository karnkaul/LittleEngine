#include "stdafx.h"
#include "Core/Logger.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"
#include "LittleEngine/Engine/EngineService.h"
#include "LittleEngine/Input/KeyboardInput.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/UI/UIElement.h"
#include "UITextInput.h"

namespace LittleEngine
{
UITextInput::UITextInput() = default;
UITextInput::~UITextInput() = default;

UITextInput* UITextInput::SetTextColour(Colour text)
{
	m_data.text = text;
	return this;
}

UITextInput* UITextInput::SetTextStyle(UIText uiText)
{
	m_data.textStyle = std::move(uiText);
	return this;
}

UITextInput::OnEditComplete::Token UITextInput::SetOnEditComplete(OnEditComplete::Callback callback)
{
	return m_onEditComplete.Register(std::move(callback));
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

void UITextInput::OnCreated()
{
	m_uKeyboard = MakeUnique<KeyboardInput>();
	m_uKeyboard->m_bClearOnEscape = false;
	m_style = UIWidgetStyle::GetDefault1(&m_style);

	m_pRoot = AddElement<UIElement>("TextInputRoot");
	m_pRoot->SetPanel(m_style.background);
	m_pRoot->m_transform.size = m_style.widgetSize;

	m_pText = AddElement<UIElement>("TextInputText");
	m_pText->m_transform.SetParent(m_pRoot->m_transform);
	m_pText->m_transform.anchor = {-1, 1};
	m_pText->m_transform.nPosition = {-Fixed(0.95f), Fixed(0.4f)};
	m_pText->GetText()->SetPrimaryColour(m_data.text);

	m_pCursor = AddElement<UIElement>("TextInputCursor");
	m_pCursor->m_transform.SetParent(m_pRoot->m_transform);
	m_pCursor->m_transform.anchor = {-1, 1};
	m_pCursor->m_transform.nPosition = {-Fixed(0.95f), Fixed(0.4f)};
	m_pCursor->GetText()->SetPrimaryColour(m_data.text);
	m_pCursor->SetText("|");
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
					  ? Services::Engine()->Input()->Register(
							[&](const EngineInput::Frame& frame) -> bool { return OnInput(frame); })
					  : nullptr;
	}
	Colour fill = m_bWriting ? m_style.interacting.fill : m_style.selected.fill;
	m_pRoot->SetPanel(fill, m_style.selected.border, m_style.selected.outline);
	if (!m_bWriting)
	{
		m_pText->SetText(UIText(m_uKeyboard->GetLiveString(), m_data.textStyle.pixelSize,
								m_data.textStyle.colour));
		m_onEditComplete(m_uKeyboard->GetLiveString());
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
		m_pText->SetText(UIText(m_uKeyboard->GetLiveString(), m_data.textStyle.pixelSize,
								m_data.textStyle.colour));
	}
	m_pCursor->GetText()->SetEnabled(m_bShowCursor && m_bWriting);

	UIWidget::Tick(dt);

	Vector2 cursorPosition = m_pText->GetText()->GetPosition();
	Fixed xOffset =
		m_uKeyboard->m_liveLine.GetCursorNPos() * m_pText->GetText()->GetBounds().GetSize().x;
	cursorPosition.x += xOffset;
	m_pCursor->GetText()->SetPosition(cursorPosition);
}

bool UITextInput::OnInput(const EngineInput::Frame& frame)
{
	if (m_bWriting)
	{
		if (frame.IsReleased(GameInputType::Enter))
		{
			m_prevText = m_uKeyboard->GetLiveString();
			OnInteractEnd(true);
			return true;
		}
		if (frame.IsReleased(GameInputType::Back))
		{
			m_uKeyboard->m_liveLine.Set(m_prevText);
			OnInteractEnd(true);
			return true;
		}
	}
	m_uKeyboard->Update(frame);
	return true;
}
} // namespace LittleEngine
