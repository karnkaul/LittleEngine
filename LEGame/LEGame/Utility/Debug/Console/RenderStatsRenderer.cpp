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
void UpdateRenderStat(bool bEnabled, UIElement& element, const char* prefix, u32 stat)
{
	if (bEnabled)
	{
		String text = String(prefix) + Strings::ToString(stat);
		element.SetText(UIText(text, 11, g_logTextColour));
	}
	element.GetText()->SetEnabled(bEnabled);
}
} // namespace

bool RenderStatsRenderer::s_bConsoleRenderStatsEnabled = false;
TweakBool(renderStats, &RenderStatsRenderer::s_bConsoleRenderStatsEnabled);

RenderStatsRenderer::RenderStatsRenderer(LEContext& context)
{
	f32 d = 0.03f;
	f32 end = 0.96f - (7 * d);
	LERenderer* pRenderer = context.Renderer();

	m_uPrimitiveCount = MakeUnique<UIElement>(LAYER_TOP, true);
	m_uPrimitiveCount->OnCreate(context, "RenderStatsPrimCount");
	m_uPrimitiveCount->GetText()
		->SetPivot({-1, 0})
		->SetPosition(pRenderer->Project({Fixed(0.82f), -Fixed(end)}, false))
		->SetEnabled(false);
	end += d;

	m_uDisabledCount = MakeUnique<UIElement>(LAYER_TOP, true);
	m_uDisabledCount->OnCreate(context, "RenderStatsDisabledCount");
	m_uDisabledCount->GetText()
		->SetPivot({-1, 0})
		->SetPosition(pRenderer->Project({Fixed(0.82f), -Fixed(end)}, false))
		->SetEnabled(false);
	end += d;

	m_uDynamicCount = MakeUnique<UIElement>(LAYER_TOP, true);
	m_uDynamicCount->OnCreate(context, "RenderStatsDynamicCount");
	m_uDynamicCount->GetText()
		->SetPivot({-1, 0})
		->SetPosition(pRenderer->Project({Fixed(0.82f), -Fixed(end)}, false))
		->SetEnabled(false);
	end += d;

	m_uStaticCount = MakeUnique<UIElement>(LAYER_TOP, true);
	m_uStaticCount->OnCreate(context, "RenderStatsStaticCount");
	m_uStaticCount->GetText()
		->SetPivot({-1, 0})
		->SetPosition(pRenderer->Project({Fixed(0.82f), -Fixed(end)}, false))
		->SetEnabled(false);
	end += d;

	m_uQuadCount = MakeUnique<UIElement>(LAYER_TOP, true);
	m_uQuadCount->OnCreate(context, "RenderStatsQuadCount");
	m_uQuadCount->GetText()
		->SetPivot({-1, 0})
		->SetPosition(pRenderer->Project({Fixed(0.82f), -Fixed(end)}, false))
		->SetEnabled(false);
	end += d;

	m_uTickRate = MakeUnique<UIElement>(LAYER_TOP, true);
	m_uTickRate->OnCreate(context, "RenderStatsTickRate");
	m_uTickRate->GetText()
		->SetPivot({-1, 0})
		->SetPosition(pRenderer->Project({Fixed(0.82f), -Fixed(end)}, false))
		->SetEnabled(true);
	end += d;

	m_uFPS = MakeUnique<UIElement>(LAYER_TOP, true);
	m_uFPS->OnCreate(context, "RenderStastFPS");
	m_uFPS->GetText()
		->SetPivot({-1, 0})
		->SetPosition(pRenderer->Project({Fixed(0.85f), -Fixed(end)}, false))
		->SetEnabled(false);
	end += d;
}

RenderStatsRenderer::~RenderStatsRenderer() = default;

void RenderStatsRenderer::Tick(Time /*dt*/)
{
	u32 ticksPerSec = 1.0f / g_renderData.tickRate.AsSeconds();
	UpdateRenderStat(s_bConsoleRenderStatsEnabled, *m_uTickRate, "Ticks/s: ", ticksPerSec);
	UpdateRenderStat(s_bConsoleRenderStatsEnabled, *m_uPrimitiveCount, "Primitives: ", g_renderData.primitiveCount);
	UpdateRenderStat(s_bConsoleRenderStatsEnabled, *m_uDisabledCount, "Disabled: ", g_renderData.disabledCount);
	UpdateRenderStat(s_bConsoleRenderStatsEnabled, *m_uFPS, "FPS: ", g_renderData.framesPerSecond);
	UpdateRenderStat(s_bConsoleRenderStatsEnabled, *m_uDynamicCount, "Dynamic: ", g_renderData.dynamicCount);
	UpdateRenderStat(s_bConsoleRenderStatsEnabled, *m_uStaticCount, "Static: ", g_renderData.staticCount);
	UpdateRenderStat(s_bConsoleRenderStatsEnabled, *m_uQuadCount, "Quads: ", g_renderData.quadCount);
}

#if DEBUGGING
VersionRenderer::VersionRenderer(LEContext& context)
{
	LERenderer* pRenderer = context.Renderer();
	const Core::Version& engineVersion = GameConfig::GetEngineVersion();

	m_uEngineVersion = MakeUnique<UIElement>(LAYER_TOP, true);
	m_uEngineVersion->OnCreate(context, "EngineVersion");
	m_uEngineVersion->SetText(UIText(engineVersion.ToString(), 10, g_logTextColour));
	m_uEngineVersion->GetText()->bDebugThisPrimitive = true;
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