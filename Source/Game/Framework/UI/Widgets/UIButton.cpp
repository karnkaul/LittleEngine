#include "Core/Logger.h"
#include "SFMLAPI/Rendering/Primitives/SFText.h"
#include "SFMLAPI/Rendering/Primitives/SFRect.h"
#include "Model/UI/UIElement.h"
#include "UIButton.h"

namespace LittleEngine
{
void UIButton::SetText(UIText uiText)
{
	m_pRoot->SetText(std::move(uiText));
	m_pRoot->Text()->SetPivot({0, -Fixed::OneHalf});
	if (m_bAutoResize && !m_pRoot->Text()->Text().empty())
	{
		Resize();
	}
}

Token UIButton::AddCallback(UIButton::OnClick::Callback callback)
{
	return m_OnInteracted.Register(std::move(callback));
}

void UIButton::OnCreated()
{
	SetType("UIButton");
	m_pRoot->Rect()->SetSize(m_style.widgetSize);
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

	m_pRoot->Text()->SetPivot({0, -Fixed::OneHalf});
}

void UIButton::Resize()
{
	const Vector2 padding(50, 20);
	auto textSize = m_pRoot->Text()->GameBounds().Size();
	auto rectSize = m_pRoot->Rect()->Size();
	Vector2 newSize = m_style.widgetSize;
	bool bSetNewSize = false;
	if (textSize.x + padding.x > rectSize.x)
	{
		newSize.x = textSize.x + padding.x;
		bSetNewSize = true;
	}
	if (textSize.y + padding.y > rectSize.y)
	{
		newSize.y = textSize.y + padding.y;
		bSetNewSize = true;
	}
	if (bSetNewSize)
	{
		m_pRoot->SetRectSize(newSize);
	}
}
} // namespace LittleEngine
