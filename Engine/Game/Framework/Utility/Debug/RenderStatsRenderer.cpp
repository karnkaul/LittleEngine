#include "Core/BuildVersion.h"
#include "RenderStatsRenderer.h"
#if ENABLED(RENDER_STATS)
#include "SFMLAPI/Rendering/RenderStats.h"
#include "SFMLAPI/Rendering/Primitives.h"
#include "Engine/GFX.h"
#include "Engine/Debug/Tweakable.h"
#include "Engine/Context/LEContext.h"
#include "Engine/Rendering/LERenderer.h"
#include "Engine/Repository/LERepository.h"

namespace LittleEngine::Debug
{
namespace
{
u16 textSize = 18;
} // namespace

bool RenderStatsRenderer::s_bConsoleRenderStatsEnabled = false;
TweakBool(renderStats, &RenderStatsRenderer::s_bConsoleRenderStatsEnabled);

RenderStatsRenderer::RenderStatsRenderer(LEContext& context)
{
	Vector2 size(300, 220);
	Colour bg(20, 20, 20, 230);
	LERenderer* pRenderer = context.Renderer();
	LayerID layer = static_cast<LayerID>(ToS32(LayerID::Top) + 2);
	auto pFont = g_pDefaultFont;

	m_pBG = pRenderer->New<SFRect>(static_cast<LayerID>(ToS32(layer) - 1));
	m_pBG->SetSize(size)->SetPivot({1, -1}, true)->SetColour(bg, true)->SetStatic(true)->SetEnabled(s_bConsoleRenderStatsEnabled);

	m_pTitles = pRenderer->New<SFText>(layer);
	m_pTitles->SetText("FPS\nQuads\nDraw Calls\nDynamic\nStatic\nAvg dt\nGame Frame\nRender Frame")
		->SetSize(textSize)
		->SetFont(*pFont)
		->SetPivot({-1, 1}, true)
		->SetColour(g_logTextColour, true)
		->SetStatic(true)
		->SetEnabled(s_bConsoleRenderStatsEnabled);

	m_pValues = pRenderer->New<SFText>(layer);
	m_pValues->SetFont(*pFont)
		->SetSize(textSize)
		->SetPivot({-1, 1}, true)
		->SetColour(g_logTextColour, true)
		->SetEnabled(s_bConsoleRenderStatsEnabled);

	// Position
	Fixed dx(150);
	Vector2 pad(-40, 40);

	Vector2 pos = g_pGFX->UIProjection({1, -1}) + pad;
	m_pBG->SetPosition(pos, true);
	Vector2 bgSize = m_pBG->Size();
	pos.x -= (bgSize.x + pad.x);
	pos.y += (bgSize.y - Fixed(10));
	m_pTitles->SetPosition(pos, true);
	pos.x += dx;
	m_pValues->SetPosition(pos, true);
}

RenderStatsRenderer::~RenderStatsRenderer()
{
	m_pBG->Destroy();
	m_pTitles->Destroy();
	m_pValues->Destroy();
}

void RenderStatsRenderer::Update()
{
	m_pBG->SetEnabled(s_bConsoleRenderStatsEnabled);
	m_pTitles->SetEnabled(s_bConsoleRenderStatsEnabled);
	m_pValues->SetEnabled(s_bConsoleRenderStatsEnabled);
	if (s_bConsoleRenderStatsEnabled)
	{
		f32 avgDT = 0.0f;
		for (auto dt : g_renderStats.dtList)
		{
			avgDT += (dt.AsSeconds() * 1000);
		}
		avgDT /= (g_renderStats.dtList.empty() ? 1 : g_renderStats.dtList.size());
		static char buf[256];
		SPRINTF(buf, sizeof(buf), "%u/%u\n%u\n%u\n%u\n%u\n%.1fms\n%u\n%u", g_renderStats.framesPerSecond, g_renderStats.fpsMax,
				g_renderStats.quadCount, g_renderStats.drawCallCount, g_renderStats.dynamicCount, g_renderStats.staticCount, avgDT,
				g_renderStats.gameFrame, g_renderStats.renderFrame);
		m_pValues->SetText(std::string(buf));
	}
}

#if defined(DEBUGGING)
const Vector2 VersionRenderer::projection = {-Fixed(0.95f), -Fixed(0.90f)};

VersionRenderer::VersionRenderer(LEContext& context)
{
	LERenderer* pRenderer = context.Renderer();
	auto pFont = g_pRepository->Load<FontAsset>("Fonts/UIFont.ttf");
	m_pBuildVersion = pRenderer->New<SFText>(LayerID::Top);
	m_pBuildVersion->SetText(std::string(BUILD_VERSION_FULL))
		->SetSize(textSize + 5U)
		->SetFont(pFont ? *pFont : *g_pDefaultFont)
		->SetPivot({-1, 0})
		->SetPosition(g_pGFX->UIProjection(projection))
		->SetColour(g_logTextColour)
		->SetEnabled(true);
}

VersionRenderer::~VersionRenderer()
{
	m_pBuildVersion->Destroy();
}
#endif
} // namespace LittleEngine::Debug
#endif
