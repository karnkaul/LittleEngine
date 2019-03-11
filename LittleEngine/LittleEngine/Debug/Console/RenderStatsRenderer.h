#pragma once
#include "CoreTypes.h"
#if ENABLED(CONSOLE)
#include "DebugConsole.h"
#include "LittleEngine/UI/UIElement.h"
#include "SFMLAPI/Rendering/Colour.h"

namespace LittleEngine
{
namespace Debug
{
class RenderStatsRenderer
{
public:
	static bool s_bConsoleRenderStatsEnabled;

private:
	UPtr<UIElement> m_uPrimitiveCount;
	UPtr<UIElement> m_uStaticCount;
	UPtr<UIElement> m_uDynamicCount;
	UPtr<UIElement> m_uFPS;

public:
	RenderStatsRenderer();
	void Tick(Time dt);
};

#if !SHIPPING
class VersionRenderer
{
private:
	UPtr<UIElement> m_uEngineVersion;
	UPtr<UIElement> m_uGameVersion;

public:
	VersionRenderer();
};
#endif
} // namespace Debug
} // namespace LittleEngine
#endif