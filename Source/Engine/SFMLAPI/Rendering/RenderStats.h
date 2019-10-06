#include <list>
#include "Core/CoreTypes.h"

#if ENABLED(RENDER_STATS)
struct RenderStats
{
	using Time = Core::Time;

	std::list<Time> dtList;
	Time lastRenderTime;
	u32 drawCallCount = 0;
	u32 quadCount = 0;
	u32 _quadCount_Internal = 0; // Unstable to render
	u32 staticCount = 0;
	u32 dynamicCount = 0;
	u32 fpsMax = 0;
	u32 framesPerSecond = 0;
	u32 renderFrame = 0;
	u32 gameFrame = 0;
};
extern RenderStats g_renderStats;
#endif
