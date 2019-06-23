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
	bool bStartThread;
};

// \warning: Ensure Viewport remains alive until destruction is complete!
class LERenderer final : public Renderer, public PrimitiveFactory
{
private:
	RendererData m_data;
	Map<u32, ViewportSize> m_viewportSizes;
	OS::Threads::Handle m_threadHandle;
	std::atomic<bool> m_bPauseRendering = true; // Used to reduce inexplicable lock contention on main thread
	
public:
	LERenderer(class Viewport& viewport, RendererData data);
	~LERenderer() override;

	void RecreateViewport(struct ViewportRecreateData data);
	const Map<u32, ViewportSize>& GetValidViewportSizes() const;
	struct ViewportSize* TryGetViewportSize(u32 height);
	Vector2 GetViewSize() const;
	Vector2 Project(Vector2 nPos, bool bPreClamp) const;

	void Lock_Swap();

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

private:
	friend class LEContext;
};
} // namespace LittleEngine
