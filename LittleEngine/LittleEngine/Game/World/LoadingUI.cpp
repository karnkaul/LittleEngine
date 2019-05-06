#include "stdafx.h"
#include "Core/Logger.h"
#include "LoadingUI.h"
#include "SFMLAPI/System/SFAssets.h"
#include "LittleEngine/Engine/EngineService.h"
#include "LittleEngine/GFX/GFX.h"
#include "LittleEngine/Repository/EngineRepository.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/UI/UIElement.h"
#include "LittleEngine/UI/UIProgressBar.h"
#include "LittleEngine/UI/UIText.h"

namespace LittleEngine
{
namespace
{
constexpr u32 MAX_ELLIPSIS_SIZE = 10;
Time ellipsisFreq = Time::Seconds(0.25f);
Fixed progressTarget;
Fixed progressNow;
} // namespace

LoadingUI::LoadingUI()
{
	String titleText = "Loading";
	FontAsset* pFont = Services::Engine()->Repository()->GetDefaultFont();
	u32 titleSize = 75;
#if DEBUG_LOGGING
	titleText = "LittleEngine is Loading Assets";
	titleSize = 50;
#endif
	LOG_D("[Loading UI] constructed");
	m_uBG = MakeUnique<UIElement>("Loading BG");
	m_uBG->SetPanel(Colour(80, 30, 100, 150));
	m_uTitle = MakeUnique<UIElement>("Loading Title", static_cast<LayerID>(m_uBG->GetLayer() + 1));
	m_uTitle->SetFont(*pFont);
	m_uTitle->SetText(UIText(titleText, titleSize, Colour::White));
	m_uTitle->m_transform.nPosition = {0, Fixed(0.1f)};
	m_uEllipsis = MakeUnique<UIElement>("Loading Ellipsis");
	m_uEllipsis->SetFont(*pFont);
	m_uEllipsis->SetText(UIText(m_ellipsis, 40, Colour::White));
	m_uEllipsis->m_transform.nPosition = {0, -Fixed(0.10f)};
	m_uProgressBar = MakeUnique<UIProgressBar>(LAYER_UI, "Asset Load Progress");
	Vector2 size(GFX::GetViewSize().x, 10);
	m_uProgressBar->InitProgressBar(size, Colour::White);
	m_uProgressBar->m_transform.padding = {0, size.y * Fixed::OneHalf};
	m_uProgressBar->m_transform.nPosition = {-Fixed::One, -Fixed::One};
	m_uProgressBar->Tick(Time::Zero);
}

LoadingUI::~LoadingUI()
{
	m_uBG = nullptr;
	m_uTitle = nullptr;
	m_uProgressBar = nullptr;
	LOG_D("[Loading UI] destroyed");
}

void LoadingUI::Tick(Time dt, Fixed progress)
{
	progressTarget = progress;
	if (progressTarget == Fixed::One)
	{
		progressNow = progressTarget;
	}
	if (progressNow < progressTarget)
	{
		progressNow += Fixed(dt.AsSeconds() * 2);
	}
	m_uProgressBar->SetProgress(progressNow);

	m_elapsed += dt;
	if (m_elapsed >= ellipsisFreq)
	{
		m_elapsed = Time::Zero;
		m_ellipsis += ". ";
		if (m_ellipsis.size() > MAX_ELLIPSIS_SIZE)
		{
			m_ellipsis = ". ";
		}
		m_uEllipsis->SetText(UIText(m_ellipsis, 40, Colour::White));
	}

	TickElements(dt);
}

void LoadingUI::Reset()
{
	progressNow = Fixed::Zero;
}

void LoadingUI::TickElements(Time dt)
{
	m_uProgressBar->Tick(dt);
	m_uBG->Tick(dt);
	m_uTitle->Tick(dt);
	m_uEllipsis->Tick(dt);

	progressTarget = Fixed::Zero;
}
} // namespace LittleEngine
