#include "stdafx.h"
#include "Core/Logger.h"
#include "SFMLAPI/Rendering/Primitives/SFRect.h"
#include "UIProgressBar.h"

namespace LittleEngine
{
UIProgressBar::UIProgressBar(LayerID layer, bool bSilent)
	: UIElement(layer, bSilent)
{
}

void UIProgressBar::InitProgressBar(Vector2 size, Colour colour, Fixed initProgress)
{
	m_size = size;
	m_transform.anchor = {-1, 0};
	m_transform.nPosition = {-1, 0};
	m_transform.size = m_size;
	SetPanel(colour);
	SetProgress(initProgress);
	GetRect()->SetStatic(false);
}

void UIProgressBar::SetProgress(Fixed progress)
{
	Fixed width = m_size.x * Maths::Clamp01(progress);
	m_transform.size.x = width;
}

void UIProgressBar::OnCreated()
{
	SetType("UIProgressBar");
}
} // namespace LittleEngine
