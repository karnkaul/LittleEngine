#pragma once
#include "Core/CoreTypes.h"
#include "Core/Jobs.h"
#include "Core/OS.h"
#include "SFMLAPI/Rendering/LayerID.h"
#include "SFMLAPI/Rendering/Renderer.h"
#include "SFMLAPI/Rendering/PrimitiveFactory.h"

namespace LittleEngine
{
struct RendererData
{
	Time tickRate;
	Time threadStartDelay;
	u32 maxFPS = 300;
	bool bStartThread = true;
};

// \warning: Ensure Viewport remains alive until destruction is complete!
class LERenderer final : public Renderer, public PrimitiveFactory
{
private:
	RendererData m_data;
	OS::Threads::Handle m_threadHandle;
	std::mutex m_swapMutex;

public:
	LERenderer(class Viewport& viewport, RendererData data);
	~LERenderer() override;

	void RecreateViewport(struct ViewportRecreateData data);
	
	void Lock_Swap();

	void Render(Fixed alpha);
	void StopRenderThread();

	bool IsRunningRenderThread() const;

private:
	void Start();
	void Stop();
	void Async_Run(Time startDelay);

#if defined(DEBUGGING)
	void ModifyTickRate(Time newTickRate);
#endif

private:
	friend class LEContext;
};
} // namespace LittleEngine
