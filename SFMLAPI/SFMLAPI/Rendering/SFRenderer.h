#pragma once
#include <atomic>
#include "SimpleTime.h"
#include "SFMLAPI/Windowing/SFWindowData.h"

namespace LittleEngine
{
using Time = Core::Time;

#if ENABLED(RENDER_STATS)
struct RenderData
{
  	Time lastRenderTime;
	u32 primitiveCount = 0;
	u32 staticCount = 0;
	u32 disabledCount = 0;
	u32 dynamicCount = 0;
	u32 framesPerSecond = 0;
};

extern RenderData g_renderData;
#endif

// \brief: Base Renderer that uses an atomic conditional bool to allow derived classes to be asynchronous
class SFRenderer
{
public:
	std::atomic<bool> m_bRendering;

protected:
	class SFWindow* m_pSFWindow;
	
public:
	SFRenderer(SFWindow& sfWindow);
	virtual ~SFRenderer();

	void Render(class IRenderBuffer& buffer, Fixed alpha);
};
} // namespace LittleEngine
