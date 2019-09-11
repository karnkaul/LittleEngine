#pragma once
#include "Core/CoreTypes.h"
#if ENABLED(RENDER_STATS)
#include "Console/DebugConsole.h"

namespace LittleEngine
{
class UIElement;
class LEContext;
class SFText;
class SFRect;

namespace Debug
{
class RenderStatsRenderer
{
public:
	static bool s_bConsoleRenderStatsEnabled;

private:
	SFRect* m_pBG;
	SFText* m_pTitles;
	SFText* m_pValues;

public:
	RenderStatsRenderer(LEContext& context);
	~RenderStatsRenderer();

	void Tick(Time dt);
};

#if defined(DEBUGGING)
class VersionRenderer
{
private:
	SFText* m_pBuildVersion;

public:
	VersionRenderer(LEContext& context);
	~VersionRenderer();
};
#endif
} // namespace Debug
} // namespace LittleEngine
#endif
