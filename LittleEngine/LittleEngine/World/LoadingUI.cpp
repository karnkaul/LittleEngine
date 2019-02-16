#include "stdafx.h"
#include "LoadingUI.h"
#include "LittleEngine/Services/Services.h"

namespace LittleEngine
{
LoadingUI::LoadingUI()
{
	String titleText = "Loading";
	u32 titleSize = 75;
#if DEBUG_LOGGING
	titleText = "LittleEngine is Loading Assets";
	titleSize = 50;
#endif
	LOG_D("[Loading UI] constructed");
	m_uBG = MakeUnique<UIElement>("Loading BG");
	m_uBG->SetPanel(Colour(80, 30, 100, 150));
	m_uTitle = MakeUnique<UIElement>("Loading Title");
	m_uTitle->m_layer = static_cast<LayerID>(m_uBG->m_layer + 1);
	m_uTitle->SetFont(*Services::Engine()->Repository()->GetDefaultFont());
	m_uTitle->SetText(UIText(titleText, titleSize, Colour::White));
	m_uTitle->m_transform.nPosition = {0, Fixed(0.1f)};
	m_uEllipses = MakeUnique<UIElement>("Loading Ellpises");
	m_uEllipses->m_layer = static_cast<LayerID>(m_uBG->m_layer + 1);
	m_uEllipses->m_transform.nPosition = {0, -Fixed(0.05f)};
}

LoadingUI::~LoadingUI()
{
	m_uBG = nullptr;
	m_uTitle = nullptr;
	m_uEllipses = nullptr;
	LOG_D("[Loading UI] destroyed");
}

void LoadingUI::Tick(Time dt)
{
	m_ellipsesElapsed += dt;

	// Animate
	if (m_ellipsesElapsed.AsMilliseconds() > 250)
	{
		m_ellipsesText += ".";
		if (m_ellipsesText.size() > 30)
			m_ellipsesText = "";
		m_ellipsesElapsed = Time::Zero;
		m_uEllipses->SetText(UIText(m_ellipsesText, 50, Colour::White));
	}

	TickElements(dt);
}

void LoadingUI::TickElements(Time dt)
{
	m_uBG->Tick(dt);
	m_uTitle->Tick(dt);
	m_uEllipses->Tick(dt);
}
} // namespace LittleEngine
