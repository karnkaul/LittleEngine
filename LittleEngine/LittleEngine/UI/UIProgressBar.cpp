#include "stdafx.h"
#include "UIProgressBar.h"

namespace LittleEngine
{
UIProgressBar::UIProgressBar(bool bSilent) : UIElement("Progress Bar", bSilent)
{
}
UIProgressBar::UIProgressBar(const String& name, bool bSilent) : UIElement(name, bSilent)
{
}

UIProgressBar::~UIProgressBar() = default;

void UIProgressBar::InitProgressBar(const Vector2& size, Colour colour, const Fixed& initProgress)
{
	m_size = size;
	m_transform.anchor = {-1, 0};
	m_transform.nPosition = {-1, 0};
	m_transform.size = m_size;
	SetPanel(colour);
	SetProgress(initProgress);
}

void UIProgressBar::SetProgress(const Fixed& progress)
{
	Fixed width = m_size.x * Maths::Clamp01(progress);
	m_transform.size.x = width;
}
} // namespace LittleEngine
