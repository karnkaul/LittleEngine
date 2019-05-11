#pragma once
#include "Core/CoreTypes.h"
#include "SFMLAPI/Rendering/SFRenderer.h"
#include "SFMLAPI/Windowing/SFWindowData.h"
#include "LittleEngine/Jobs/JobHandle.h"

namespace LittleEngine
{
// \brief: Starts render loop on another thread on construction, joins thread on destruction.
// \warning: Ensure GLWindow remains alive until destruction is complete!
class AsyncRenderLoop final : public SFRenderer
{
public:
	std::atomic<bool> m_bPauseRendering = true; // Used to reduce inexplicable lock contention on main thread
private:
	JobHandle m_pRenderJobHandle;
	Time m_tickRate;
	class IRenderBuffer* m_pBuffer;
	bool m_bRunThread = false;

public:
	AsyncRenderLoop(SFWindow& sfWindow, Time tickRate, bool bStartThread);
	~AsyncRenderLoop() override;

	void RecreateWindow(SFWindowRecreateData data);

private:
	void Start();
	void Stop();
	void Async_Run();
};
} // namespace LittleEngine
