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
	static Map<u32, ViewportSize> s_viewportSizes;

private:
	RendererData m_data;
	OS::Threads::Handle m_threadHandle;
	std::mutex m_swapMutex;

public:
	static void PopulateViewportSizes(Fixed aspectRatio);
	static const Map<u32, ViewportSize>& ValidViewportSizes();
	static const ViewportSize* MaxViewportSize(bool bBorderless);
	static ViewportSize* TryGetViewportSize(u32 height);

public:
	LERenderer(class Viewport& viewport, RendererData data);
	~LERenderer() override;

	void RecreateViewport(struct ViewportRecreateData data);
	Vector2 ViewSize() const;
	Vector2 Project(Vector2 nPos, bool bPreClamp) const;

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
