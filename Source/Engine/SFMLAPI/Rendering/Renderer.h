#pragma once
#include <atomic>
#include "Core/SimpleTime.h"
#include "SFMLAPI/Viewport/ViewportData.h"

namespace LittleEngine
{
using Time = Core::Time;

#if ENABLED(RENDER_STATS)
struct RenderData
{
	Time tickRate;
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

extern RenderData g_renderData;
#endif

// \brief: Base Renderer that uses an atomic conditional bool to allow derived classes to be asynchronous
class Renderer
{
public:
	std::atomic<bool> m_bRendering;

protected:
	class Viewport* m_pViewport;

public:
	Renderer(Viewport& viewPort);
	virtual ~Renderer();

	void RenderFrame(class IRenderBuffer& buffer, Fixed alpha);
	void WaitForVsync();
};
} // namespace LittleEngine
