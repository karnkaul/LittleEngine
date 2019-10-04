#pragma once
#include "Core/CoreTypes.h"
#include "Core/Jobs.h"
#include "Core/OS.h"
#include "SFMLAPI/Rendering/LayerID.h"
#include "PrimitiveFactory.h"

namespace LittleEngine
{
// \warning: Ensure Viewport remains alive until destruction is complete!
class LERenderer final : public PrimitiveFactory
{
public:
	struct Data
	{
		Time tickRate;
		Time threadStartDelay;
		u32 maxFPS = 300;
		bool bStartThread = true;

		Data();
		Data(Time tickRate, Time threadStartDelay, u32 maxFPS, bool bStartThread);
	};

private:
	std::atomic<bool> m_bRendering;

private:
	Data m_data;
	OS::Threads::Handle m_threadHandle;
	std::mutex m_swapMutex;
	class Viewport* m_pViewport;

public:
	LERenderer(class Viewport& viewport, Data data);
	~LERenderer() override;

	bool IsRunningRenderThread() const;
	
private:
	void RecreateViewport(struct ViewportRecreateData data);
	void Lock_Swap();
	void Lock_Reconcile();
	void Render(Fixed alpha);
	void RenderFrame(class PrimitiveFactory& buffer, Fixed alpha);
	void StopRenderThread();

	void Start();
	void Stop();
	void Async_Run(Time startDelay);
	void WaitForVsync();

#if defined(DEBUGGING)
	void ModifyTickRate(Time newTickRate);
#endif

private:
	friend class LEContext;
};
} // namespace LittleEngine
