#include "stdafx.h"
#include "RenderStatsRenderer.h"
#if ENABLED(CONSOLE)
#include "LittleEngine/GFX/GFX.h"
#include "LittleEngine/Engine/EngineConfig.h"
#include "LittleEngine/Game/GameManager.h"
#include "SFMLAPI/Rendering/SFRenderer.h"

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
		element.SetText(UIText(text, 10, g_logTextColour));
	}
	element.GetText()->SetEnabled(bEnabled);
}
} // namespace

bool RenderStatsRenderer::s_bConsoleRenderStatsEnabled = false;

RenderStatsRenderer::RenderStatsRenderer()
{
	m_uPrimitiveCount = MakeUnique<UIElement>("PrimitiveCount");
	m_uPrimitiveCount->GetText()
		->SetPivot({-1, 0})
		->SetPosition(GFX::Project({Fixed(0.90f), -Fixed(0.92f)}, false))
		->SetEnabled(false);

	m_uDynamicCount = MakeUnique<UIElement>("DynamicPrimitiveCount");
	m_uDynamicCount->GetText()
		->SetPivot({-1, 0})
		->SetPosition(GFX::Project({Fixed(0.90f), -Fixed(0.94f)}, false))
		->SetEnabled(false);

	m_uStaticCount = MakeUnique<UIElement>("StaticPrimitiveCount");
	m_uStaticCount->GetText()
		->SetPivot({-1, 0})
		->SetPosition(GFX::Project({Fixed(0.90f), -Fixed(0.96f)}, false))
		->SetEnabled(false);

	m_uFPS = MakeUnique<UIElement>("FPS");
	m_uFPS->GetText()
		->SetPivot({-1, 0})
		->SetPosition(GFX::Project({Fixed(0.90f), -Fixed(0.98f)}, false))
		->SetEnabled(false);
}

void RenderStatsRenderer::Tick(Time)
{
	UpdateRenderStat(s_bConsoleRenderStatsEnabled, *m_uPrimitiveCount, "Primitives: ", g_renderData.primitiveCount);
	UpdateRenderStat(s_bConsoleRenderStatsEnabled, *m_uFPS, "FPS: ", g_renderData.framesPerSecond);
	UpdateRenderStat(s_bConsoleRenderStatsEnabled, *m_uDynamicCount, "Dynamic: ", g_renderData.dynamicCount);
	UpdateRenderStat(s_bConsoleRenderStatsEnabled, *m_uStaticCount, "Static: ", g_renderData.staticCount);
}

#if !SHIPPING
VersionRenderer::VersionRenderer()
{
	const Core::Version& engineVersion = EngineConfig::GetEngineVersion();
	m_uEngineVersion = MakeUnique<UIElement>("EngineVersion");
	m_uEngineVersion->SetText(UIText(engineVersion.ToString(), 10, g_logTextColour));
	m_uEngineVersion->GetText()
		->SetPivot({-1, 0})
		->SetPosition(GFX::Project({-Fixed(0.99f), -Fixed(0.97f)}, false))
		->SetEnabled(true);

	const Core::Version& gameVersion = GameManager::GetGameVersion();
	m_uGameVersion = MakeUnique<UIElement>("GameVersion");
	m_uGameVersion->SetText(UIText(gameVersion.ToString(), 11, g_logTextColour));
	m_uGameVersion->GetText()
		->SetPivot({-1, 0})
		->SetPosition(GFX::Project({-Fixed(0.99f), -Fixed(0.95f)}, false))
		->SetEnabled(true);
}
#endif
} // namespace Debug
} // namespace LittleEngine
#endif