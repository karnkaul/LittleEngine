#pragma once
#include "Core/CoreTypes.h"
#if ENABLED(RENDER_STATS)
#include "DebugConsole.h"

namespace LittleEngine
{
class UIElement;
class LEContext;

namespace Debug
{
class RenderStatsRenderer
{
public:
	static bool s_bConsoleRenderStatsEnabled;

private:
	UPtr<UIElement> m_uBG;
	UPtr<UIElement> m_uTitles;
	UPtr<UIElement> m_uValues;

public:
	RenderStatsRenderer(LEContext& context);
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
	VersionRenderer(LEContext& context);
	~VersionRenderer();
};
#endif
} // namespace Debug
} // namespace LittleEngine
#endif