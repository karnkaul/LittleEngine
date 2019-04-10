#include "stdafx.h"
#include "Logger.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"
#include "UIProgressBar.h"

namespace LittleEngine
{
UIProgressBar::UIProgressBar(bool bSilent) : UIElement("Progress Bar", bSilent)
{
	SetName("", "UIProgressBar");
}
UIProgressBar::UIProgressBar(String name, bool bSilent) : UIElement(std::move(name), bSilent)
{
	SetName("", "UIProgressBar");
}

UIProgressBar::~UIProgressBar() = default;

void UIProgressBar::InitProgressBar(Vector2 size, Colour colour, Fixed initProgress)
{
	m_size = size;
	m_transform.anchor = {-1, 0};
	m_transform.nPosition = {-1, 0};
	m_transform.size = m_size;
	SetPanel(colour);
	SetProgress(initProgress);
	GetPrimitive()->SetStatic(false);
}

void UIProgressBar::SetProgress(Fixed progress)
{
	Fixed width = m_size.x * Maths::Clamp01(progress);
	m_transform.size.x = width;
}
} // namespace LittleEngine
