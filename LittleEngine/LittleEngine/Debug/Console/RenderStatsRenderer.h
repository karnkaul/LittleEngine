#pragma once
#include "Core/CoreTypes.h"
#if ENABLED(RENDER_STATS)
#include "DebugConsole.h"

namespace LittleEngine
{
class UIElement;

namespace Debug
{
class RenderStatsRenderer
{
public:
	static bool s_bConsoleRenderStatsEnabled;

private:
	UPtr<UIElement> m_uPrimitiveCount;
	UPtr<UIElement> m_uDisabledCount;
	UPtr<UIElement> m_uStaticCount;
	UPtr<UIElement> m_uDynamicCount;
	UPtr<UIElement> m_uQuadCount;
	UPtr<UIElement> m_uFPS;

public:
	RenderStatsRenderer();
	~RenderStatsRenderer();
	
	void Tick(Time dt);
};

#if DEBUGGING
class VersionRenderer
{
private:
	UPtr<UIElement> m_uEngineVersion;
	UPtr<UIElement> m_uGameVersion;

public:
	VersionRenderer();
	~VersionRenderer();
};
#endif
} // namespace Debug
} // namespace LittleEngine
#endif