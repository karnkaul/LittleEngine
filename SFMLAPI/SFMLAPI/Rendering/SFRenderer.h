#pragma once
#include <atomic>
#include "SimpleTime.h"
#include "GFXBuffer.h"
#include "SFMLAPI/Windowing/SFWindowData.h"

namespace LittleEngine
{
#if ENABLED(RENDER_STATS)
struct RenderData
{
	Time lastRenderTime;
	u32 primitiveCount = 0;
	u32 staticCount = 0;
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
	Time m_tickRate;

public:
	SFRenderer(SFWindow& sfWindow, Time tickRate);
	virtual ~SFRenderer();

	void Render(GFXBuffer& buffer);
	void Display();
};
} // namespace LittleEngine
