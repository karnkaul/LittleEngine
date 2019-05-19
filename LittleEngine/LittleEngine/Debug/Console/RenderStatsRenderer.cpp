#include "stdafx.h"
#include "RenderStatsRenderer.h"
#if ENABLED(RENDER_STATS)
#include "SFMLAPI/Rendering/SFRenderer.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"
#include "LittleEngine/Debug/Console/Tweakable.h"
#include "LittleEngine/Engine/EngineConfig.h"
#include "LittleEngine/Game/GameManager.h"
#include "LittleEngine/GFX/GFX.h"
#include "LittleEngine/UI/UIElement.h"
#include "LittleEngine/UI/UIText.h"

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

RenderStatsRenderer::RenderStatsRenderer()
{
	f32 d = 0.03f;
	f32 end = 0.96f - (6 * d);
	
	m_uPrimitiveCount = MakeUnique<UIElement>("PrimitiveCount", LAYER_TOP);
	m_uPrimitiveCount->GetText()
		->SetPivot({-1, 0})
		->SetPosition(GFX::Project({Fixed(0.82f), -Fixed(end)}, false))
		->SetEnabled(false);
	end += d;

	m_uDisabledCount = MakeUnique<UIElement>("DisabledCount", LAYER_TOP);
	m_uDisabledCount->GetText()
		->SetPivot({-1, 0})
		->SetPosition(GFX::Project({Fixed(0.82f), -Fixed(end)}, false))
		->SetEnabled(false);
	end += d;

	m_uDynamicCount = MakeUnique<UIElement>("DynamicPrimitiveCount", LAYER_TOP);
	m_uDynamicCount->GetText()
		->SetPivot({-1, 0})
		->SetPosition(GFX::Project({Fixed(0.82f), -Fixed(end)}, false))
		->SetEnabled(false);
	end += d;

	m_uStaticCount = MakeUnique<UIElement>("StaticPrimitiveCount", LAYER_TOP);
	m_uStaticCount->GetText()
		->SetPivot({-1, 0})
		->SetPosition(GFX::Project({Fixed(0.82f), -Fixed(end)}, false))
		->SetEnabled(false);
	end += d;

	m_uQuadCount = MakeUnique<UIElement>("QuadCount", LAYER_TOP);
	m_uQuadCount->GetText()
		->SetPivot({-1, 0})
		->SetPosition(GFX::Project({Fixed(0.82f), -Fixed(end)}, false))
		->SetEnabled(false);
	end += d;

	m_uFPS = MakeUnique<UIElement>("FPS", LAYER_TOP);
	m_uFPS->GetText()
		->SetPivot({-1, 0})
		->SetPosition(GFX::Project({Fixed(0.85f), -Fixed(end)}, false))
		->SetEnabled(false);
}

RenderStatsRenderer::~RenderStatsRenderer() = default;

void RenderStatsRenderer::Tick(Time /*dt*/)
{
	UpdateRenderStat(s_bConsoleRenderStatsEnabled, *m_uPrimitiveCount, "Primitives: ", g_renderData.primitiveCount);
	UpdateRenderStat(s_bConsoleRenderStatsEnabled, *m_uDisabledCount, "Disabled: ", g_renderData.disabledCount);
	UpdateRenderStat(s_bConsoleRenderStatsEnabled, *m_uFPS, "FPS: ", g_renderData.framesPerSecond);
	UpdateRenderStat(s_bConsoleRenderStatsEnabled, *m_uDynamicCount, "Dynamic: ", g_renderData.dynamicCount);
	UpdateRenderStat(s_bConsoleRenderStatsEnabled, *m_uStaticCount, "Static: ", g_renderData.staticCount);
	UpdateRenderStat(s_bConsoleRenderStatsEnabled, *m_uQuadCount, "Quads: ", g_renderData.quadCount);
}

#if DEBUGGING
VersionRenderer::VersionRenderer()
{
	const Core::Version& engineVersion = EngineConfig::GetEngineVersion();
	m_uEngineVersion = MakeUnique<UIElement>("EngineVersion", LAYER_TOP);
	m_uEngineVersion->SetText(UIText(engineVersion.ToString(), 10, g_logTextColour));
	m_uEngineVersion->GetText()->bDebugThisPrimitive = true;
	m_uEngineVersion->GetText()
		->SetPivot({-1, 0})
		->SetPosition(GFX::Project({-Fixed(0.99f), -Fixed(0.97f)}, false))
		->SetEnabled(true);

	const Core::Version& gameVersion = GameManager::GetGameVersion();
	m_uGameVersion = MakeUnique<UIElement>("GameVersion", LAYER_TOP);
	m_uGameVersion->SetText(UIText(gameVersion.ToString(), 11, g_logTextColour));
	m_uGameVersion->GetText()
		->SetPivot({-1, 0})
		->SetPosition(GFX::Project({-Fixed(0.99f), -Fixed(0.95f)}, false))
		->SetEnabled(true);
}

VersionRenderer::~VersionRenderer() = default;
#endif
} // namespace Debug
} // namespace LittleEngine
#endif