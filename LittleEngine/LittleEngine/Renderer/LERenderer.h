#pragma once
#include "Core/CoreTypes.h"
#include "Core/Jobs.h"
#include "Core/OS.h"
#include "SFMLAPI/Rendering/SFLayerID.h"
#include "SFMLAPI/Rendering/SFRenderer.h"

namespace LittleEngine
{
struct RendererData
{
	Time tickRate;
	Time threadStartDelay;
	bool bStartThread;
};

// \warning: Ensure SFViewport remains alive until destruction is complete!
class LERenderer final : public SFRenderer
{
private:
	RendererData m_data;
	Map<u32, SFViewportSize> m_viewportSizes;
	UPtr<class RenderFactory> m_uFactory;	
	OS::Threads::Handle m_threadHandle;
	std::atomic<bool> m_bPauseRendering = true; // Used to reduce inexplicable lock contention on main thread
	
public:
	LERenderer(class SFViewport& sfViewport, RendererData data);
	~LERenderer() override;

	void RecreateViewport(struct SFViewportRecreateData data);
	const Map<u32, SFViewportSize>& GetValidViewportSizes() const;
	struct SFViewportSize* TryGetViewportSize(u32 height);
	Vector2 GetViewSize() const;
	Vector2 Project(Vector2 nPos, bool bPreClamp) const;

	class SFPrimitive* New(LayerID layer);

	Time GetLastSwapTime() const;
	void Lock_Swap();
	void Reconcile();
	
	void Render(Fixed alpha);
	void StopRenderThread();

	bool IsRunningRenderThread() const;

private:
	void Start();
	void Stop();
	void Async_Run(Time startDelay);

#if DEBUGGING
	void ModifyTickRate(Time newTickRate);
#endif

	friend class LEContext;
};
} // namespace LittleEngine
