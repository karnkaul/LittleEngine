#include "stdafx.h"
#include "LoadingUI.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/GFX/GFX.h"

namespace LittleEngine
{
namespace
{
Fixed progressTarget;
Fixed progressNow;
}

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
	m_uProgressBar = MakeUnique<UIProgressBar>("Asset Load Progress");
	Vector2 size(GFX::GetViewSize().x, 10);
	m_uProgressBar->InitProgressBar(size, Colour::White);
	m_uProgressBar->m_transform.nPosition = {-Fixed::One, -Fixed(0.98f)};
	m_uProgressBar->Tick(Time::Zero);
}

LoadingUI::~LoadingUI()
{
	m_uBG = nullptr;
	m_uTitle = nullptr;
	m_uProgressBar = nullptr;
	LOG_D("[Loading UI] destroyed");
}

void LoadingUI::Tick(Time dt, const Fixed& progress)
{
	progressTarget = progress;
	if (progressTarget == Fixed::One)
	{
		progressNow = progressTarget;
	}
	if (progressNow < progressTarget)
	{
		progressNow += Fixed(dt.AsSeconds());
	}
	m_uProgressBar->SetProgress(progressNow);

	TickElements(dt);
}

void LoadingUI::TickElements(Time dt)
{
	m_uProgressBar->Tick(dt);
	m_uBG->Tick(dt);
	m_uTitle->Tick(dt);

	progressTarget = Fixed::Zero;
}
} // namespace LittleEngine
