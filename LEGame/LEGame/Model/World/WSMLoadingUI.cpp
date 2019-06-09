#include "stdafx.h"
#include "Core/Logger.h"
#include "WSMLoadingUI.h"
#include "SFMLAPI/System/SFAssets.h"
#include "LittleEngine/Context/LEContext.h"
#include "LittleEngine/Repository/LERepository.h"
#include "LEGame/Model/UI/UIElement.h"
#include "LEGame/Model/UI/UIText.h"
#include "LEGame/Framework/UI/Elements/UIProgressBar.h"

namespace LittleEngine
{
namespace
{
constexpr u32 MAX_ELLIPSIS_SIZE = 10;
Time ellipsisFreq = Time::Seconds(0.25f);
Vector2 progressBarSize;
} // namespace

WSMLoadingUI::WSMLoadingUI(LEContext& context) : m_pContext(&context)
{
	String titleText = "Loading";
	FontAsset* pFont = g_pRepository->GetDefaultFont();
	u32 titleSize = 75;
#if DEBUG_LOGGING
	titleText = "LittleEngine is Loading Assets";
	titleSize = 50;
#endif
	LOG_D("[Loading UI] constructed");
	m_uBG = MakeUnique<UIElement>();
	m_uBG->OnCreate(context, "WSMLoadingBG");
	m_uBG->SetPanel(Colour(40, 15, 50));
	m_uTitle = MakeUnique<UIElement>(static_cast<LayerID>(m_uBG->GetLayer() + 1));
	m_uTitle->OnCreate(context, "WSMLoadingTitle");
	m_uTitle->SetFont(*pFont);
	m_uTitle->SetText(UIText(titleText, titleSize, Colour::White));
	m_uTitle->m_transform.nPosition = {0, Fixed(0.1f)};
	m_uEllipsis = MakeUnique<UIElement>();
	m_uEllipsis->OnCreate(context, "WSMLoadingEllipsis");
	m_uEllipsis->SetFont(*pFont);
	m_uEllipsis->SetText(UIText(m_ellipsis, 40, Colour::White));
	m_uEllipsis->m_transform.nPosition = {0, -Fixed(0.10f)};
	m_uProgressBar = MakeUnique<UIElement>();
	m_uProgressBar->OnCreate(context, "WSMAssetLoadProgress");
	progressBarSize = {m_pContext->GetViewSize().x, 10};
	m_uProgressBar->SetPanel(Colour::White);
	m_uProgressBar->m_transform.anchor = {-1, 0};
	m_uProgressBar->m_transform.padding = {0, progressBarSize.y * Fixed::OneHalf};
	m_uProgressBar->m_transform.nPosition = {-Fixed::One, -Fixed::One};
	m_uProgressBar->Tick();
}

WSMLoadingUI::~WSMLoadingUI()
{
	m_uBG = nullptr;
	m_uTitle = nullptr;
	m_uProgressBar = nullptr;
	LOG_D("[Loading UI] destroyed");
}

void WSMLoadingUI::Tick(Time dt, Fixed progress)
{
	m_uProgressBar->m_transform.size = {progress * progressBarSize.x, progressBarSize.y};
	
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

void WSMLoadingUI::Reset()
{
	m_uProgressBar->m_transform.size = {Fixed::Zero, progressBarSize.y};
	m_uProgressBar->Tick();
}

void WSMLoadingUI::TickElements(Time dt)
{
	m_uProgressBar->Tick(dt);
	m_uBG->Tick(dt);
	m_uTitle->Tick(dt);
	m_uEllipsis->Tick(dt);
}
} // namespace LittleEngine
