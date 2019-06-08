#pragma once
#include <atomic>
#include "Core/SimpleTime.h"
#include "SFMLAPI/Viewport/SFViewportData.h"

namespace LittleEngine
{
using Time = Core::Time;

#if ENABLED(RENDER_STATS)
struct RenderData
{
	Time tickRate;
  	Time lastRenderTime;
	u32 primitiveCount = 0;
	u32 staticCount = 0;
	u32 disabledCount = 0;
	u32 dynamicCount = 0;
	u32 quadCount = 0;
	u32 framesPerSecond = 0;
	std::atomic<u32> rendersPerFrame = 0;
};

extern RenderData g_renderData;
#endif

// \brief: Base Renderer that uses an atomic conditional bool to allow derived classes to be asynchronous
class SFRenderer
{
public:
	std::atomic<bool> m_bRendering;
	
protected:
	class SFViewport* m_pViewport;
	
public:
	SFRenderer(SFViewport& viewPort);
	virtual ~SFRenderer();

	void RenderFrame(class IRenderBuffer& buffer, Fixed alpha);
};
} // namespace LittleEngine
