#include "stdafx.h"
#include "RenderStatsRenderer.h"
#if ENABLED(RENDER_STATS)
#include "SFMLAPI/Rendering/SFRenderer.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"
#include "LittleEngine/Debug/Tweakable.h"
#include "LittleEngine/Context/LEContext.h"
#include "LittleEngine/Renderer/LERenderer.h"
#include "LEGame/Model/GameConfig.h"
#include "LEGame/Model/GameManager.h"
#include "LEGame/Model/UI/UIElement.h"

namespace LittleEngine
{
namespace Debug
{
namespace
{
String Combine(InitList<u32> values, String suffix)
{
	String ret;
	for (auto value : values)
	{
		ret += (Strings::ToString(value) + suffix);
	}
	return ret;
}
} // namespace

bool RenderStatsRenderer::s_bConsoleRenderStatsEnabled = false;
TweakBool(renderStats, &RenderStatsRenderer::s_bConsoleRenderStatsEnabled);

RenderStatsRenderer::RenderStatsRenderer(LEContext& context)
{
	Fixed nY(-75, 100);
	Fixed x(82, 100);
	Fixed dx(12, 100);
	LERenderer* pRenderer = context.Renderer();

	m_uTitles = MakeUnique<UIElement>(LAYER_TOP, true);
	m_uTitles->OnCreate(context, "RenderStatsTitles");
	m_uTitles->GetText()
		->SetPivot({-1, 1})
		->SetPosition(pRenderer->Project({x, nY}, false))
		->SetEnabled(false);
	m_uTitles->SetText(UIText(
		"Primitives\nDisabled\nDynamic\nStatic\nQuads\nTicks/s\nRenders/Frame\nFPS", 11, g_logTextColour));
	m_uValues = MakeUnique<UIElement>(LAYER_TOP, true);
	m_uValues->OnCreate(context, "RenderStatsValues");
	m_uValues->GetText()
		->SetPivot({-1, 1})
		->SetPosition(pRenderer->Project({x + dx, nY}, false))
		->SetEnabled(false);
}

RenderStatsRenderer::~RenderStatsRenderer() = default;

void RenderStatsRenderer::Tick(Time /*dt*/)
{
	m_uTitles->GetText()->SetEnabled(s_bConsoleRenderStatsEnabled);
	m_uValues->GetText()->SetEnabled(s_bConsoleRenderStatsEnabled);
	if (s_bConsoleRenderStatsEnabled)
	{
		u32 ticksPerSec = 1.0f / g_renderData.tickRate.AsSeconds();
		String values = Combine(
			{g_renderData.primitiveCount, g_renderData.disabledCount, g_renderData.dynamicCount,
			 g_renderData.staticCount, g_renderData.quadCount, ticksPerSec,
			 g_renderData.rendersPerFrame.load(std::memory_order_relaxed), g_renderData.framesPerSecond},
			"\n");
		m_uValues->SetText(UIText(std::move(values), 11, g_logTextColour));
	}
}

#if DEBUGGING
VersionRenderer::VersionRenderer(LEContext& context)
{
	LERenderer* pRenderer = context.Renderer();
	const Core::Version& engineVersion = GameConfig::GetEngineVersion();

	m_uEngineVersion = MakeUnique<UIElement>(LAYER_TOP, true);
	m_uEngineVersion->OnCreate(context, "EngineVersion");
	m_uEngineVersion->SetText(UIText(engineVersion.ToString(), 10, g_logTextColour));
	m_uEngineVersion->GetText()->m_bDebugThisPrimitive = true;
	m_uEngineVersion->GetText()
		->SetPivot({-1, 0})
		->SetPosition(pRenderer->Project({-Fixed(0.99f), -Fixed(0.97f)}, false))
		->SetEnabled(true);

	const Core::Version& gameVersion = GameManager::GetGameVersion();
	m_uGameVersion = MakeUnique<UIElement>(LAYER_TOP, true);
	m_uGameVersion->OnCreate(context, "GameVersion");
	m_uGameVersion->SetText(UIText(gameVersion.ToString(), 11, g_logTextColour));
	m_uGameVersion->GetText()
		->SetPivot({-1, 0})
		->SetPosition(pRenderer->Project({-Fixed(0.99f), -Fixed(0.95f)}, false))
		->SetEnabled(true);
}

VersionRenderer::~VersionRenderer() = default;
#endif
} // namespace Debug
} // namespace LittleEngine
#endif