#include "stdafx.h"
#include "RenderStatsRenderer.h"
#if ENABLED(CONSOLE)
#include "LittleEngine/GFX/GFX.h"
#include "SFMLAPI/Rendering/SFRenderer.h"

namespace LittleEngine
{
namespace Debug
{
namespace
{
void UpdateText(bool bEnabled, UIElement& element, const char* prefix, u32 stat)
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
	UpdateText(s_bConsoleRenderStatsEnabled, *m_uPrimitiveCount, "Primitives: ", g_renderData.primitiveCount);
	UpdateText(s_bConsoleRenderStatsEnabled, *m_uFPS, "FPS: ", g_renderData.framesPerSecond);
	UpdateText(s_bConsoleRenderStatsEnabled, *m_uDynamicCount, "Dynamic: ", g_renderData.dynamicCount);
	UpdateText(s_bConsoleRenderStatsEnabled, *m_uStaticCount, "Static: ", g_renderData.staticCount);
}
} // namespace Debug
} // namespace LittleEngine
#endif