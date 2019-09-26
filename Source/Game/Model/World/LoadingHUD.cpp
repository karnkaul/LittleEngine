#include "Core/Logger.h"
#include "SFMLAPI/System/Assets.h"
#include "SFMLAPI/Rendering/Primitives/Quad.h"
#include "SFMLAPI/Rendering/Primitives/SFText.h"
#include "Engine/Context/LEContext.h"
#include "Engine/Renderer/LERenderer.h"
#include "Model/GameManager.h"
#include "Model/UI/UIElement.h"
#include "LoadingHUD.h"

namespace LittleEngine
{
namespace
{
const Fixed pbHalfHeight = 4;
Fixed pbWidth;

void SetProgress(Quad& quad, Fixed progress, bool bImmediate)
{
	Vector2 tl(0, pbHalfHeight);
	Vector2 br(progress * pbWidth, -pbHalfHeight);
	quad.SetModel(Rect2{tl, br}, bImmediate);
}
} // namespace

LoadingHUD::LoadingHUD()
{
	LayerID bg = LayerID::Top;
	LayerID hud = static_cast<LayerID>(ToS32(bg) + 1);
	auto pMainFont = g_pDefaultFont;
	auto pTitleFont = PreloadAsset<FontAsset>("Fonts/Sunscreen.otf");
	auto pSubtitleFont = PreloadAsset<FontAsset>("Fonts/UIFont.ttf");
	auto pTexture = PreloadAsset<TextureAsset>("Textures/LoadingIcon.png");

	m_pBG = g_pGameManager->Renderer()->New<Quad>(bg);
	m_pTitle = g_pGameManager->Renderer()->New<SFText>(hud);
	m_pSubtitle = g_pGameManager->Renderer()->New<SFText>(hud);
	m_pProgressBar = g_pGameManager->Renderer()->New<Quad>(hud);
	m_pRotator = g_pGameManager->Renderer()->New<Quad>(hud);

	m_pBG->SetPrimaryColour(Colour(20, 5, 30, 0))->SetEnabled(true);
	m_pTitle->SetFont(pTitleFont ? *pTitleFont : *pMainFont);
	m_pSubtitle->SetFont(pSubtitleFont ? *pSubtitleFont : pTitleFont ? *pTitleFont : *pMainFont);
	m_pTitle->SetPosition({0, 70})->SetEnabled(true);
	m_pSubtitle->SetPosition({0, -30})->SetEnabled(true);

	m_pProgressBar->SetPrimaryColour(Colour::White)->SetEnabled(true);

	m_pRotator->SetModel(Rect2::SizeCentre({75, 75}), true)->SetEnabled(true);
	if (pTexture)
	{
		m_pRotator->SetTexture(*pTexture);
	}

	// Layout
	Vector2 viewSize = g_pGFX->UISpace();
	Vector2 halfView = viewSize * Fixed::OneHalf;
	Vector2 rotatorPos = halfView - Vector2(100, 100);
	pbWidth = viewSize.x;
	m_pBG->SetModel(Rect2::SizeCentre(viewSize), true);
	SetProgress(*m_pProgressBar, Fixed::Zero, true);
	m_pProgressBar->SetPosition({-halfView.x, -halfView.y + pbHalfHeight}, true);
	m_pRotator->SetPosition(rotatorPos, true);

	SetEnabled(false);
	LOG_D("[Loading UI] constructed");
}

LoadingHUD::~LoadingHUD()
{
	m_pProgressBar->Destroy();
	m_pBG->Destroy();
	m_pTitle->Destroy();
	m_pSubtitle->Destroy();
	m_pRotator->Destroy();
	LOG_D("[Loading UI] destroyed");
}

SFText& LoadingHUD::Title() const
{
	return *m_pTitle;
}

SFText* LoadingHUD::Subtitle() const
{
	return m_pSubtitle;
}

Quad* LoadingHUD::Spinner() const
{
	return m_pRotator;
}

void LoadingHUD::Tick(Time dt, Fixed progress)
{
	m_alpha = Maths::Lerp(m_alpha, m_alphaTarget, Fixed(dt.AsSeconds() * 8));
	SetProgress(*m_pProgressBar, progress, false);

	m_elapsed += dt;
	m_angle -= Fixed(dt.AsSeconds() * 200.0f);
	m_pRotator->SetOrientation(Vector2::ToOrientation(m_angle));

	Update(false);
}

void LoadingHUD::Reset()
{
	auto pTexture = g_pRepository->Load<TextureAsset>("Textures/LoadingIcon.png");
	if (pTexture)
	{
		m_pRotator->SetTexture(*pTexture);
	}
	SetProgress(*m_pProgressBar, Fixed::Zero, true);
}

void LoadingHUD::SetEnabled(bool bEnabled)
{
	if (bEnabled)
	{
		m_alpha = m_alphaTarget = Fixed::One;
	}
	else
	{
		m_alphaTarget = Fixed::Zero;
	}
	Update(true);
}

void LoadingHUD::Update(bool bImmediate)
{
	UByte alpha(static_cast<u8>((m_alpha * 255).ToU32()));
	if (alpha.rawValue < 5)
	{
		alpha.rawValue = 0;
	}
	else if (alpha.rawValue > 250)
	{
		alpha.rawValue = 255;
	}
	m_pBG->SetPrimaryAlpha(alpha, bImmediate);
	m_pProgressBar->SetPrimaryAlpha(alpha, bImmediate);
	m_pTitle->SetPrimaryAlpha(alpha, bImmediate);
	m_pSubtitle->SetPrimaryAlpha(alpha, bImmediate);
	m_pRotator->SetPrimaryAlpha(alpha, bImmediate);
}
} // namespace LittleEngine
