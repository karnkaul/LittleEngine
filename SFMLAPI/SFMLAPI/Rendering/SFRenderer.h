#pragma once
#include <atomic>
#include "GFXBuffer.h"
#include "SFMLAPI/Windowing/SFWindowData.h"
#include "SFMLAPI/System/SFTime.h"

namespace LittleEngine
{
struct RenderData
{
	Time lastRenderTime;
	u32 primitiveCount = 0;
	u32 staticCount = 0;
	u32 dynamicCount = 0;
	u32 framesPerSecond = 0;

	void Reset();
};

extern RenderData g_renderData;

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

	void Render(GFXBuffer& buffer);
	void Display();

	void SetWindowSize(const SFWindowSize& size);
};
} // namespace LittleEngine
