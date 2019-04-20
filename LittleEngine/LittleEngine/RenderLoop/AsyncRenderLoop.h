#pragma once
#include "CoreTypes.h"
#include "SFMLAPI/Rendering/SFRenderer.h"
#include "SFMLAPI/Windowing/SFWindowData.h"

namespace LittleEngine
{
// \brief: Starts render loop on another thread on construction, joins thread on destruction.
// \warning: Ensure GLWindow remains alive until destruction is complete!
class AsyncRenderLoop final : public SFRenderer
{
private:
	SPtr<class JobHandle> m_pRenderJobHandle;
	Time m_tickRate;
	class IRenderBuffer* m_pBuffer;
	bool m_bRunThread = false;

public:
	AsyncRenderLoop(SFWindow& sfWindow, IRenderBuffer& renderBuffer, Time tickRate, bool bStartThread);
	~AsyncRenderLoop() override;

	void RecreateWindow(SFWindowRecreateData data);
	
private:
	void Start();
	void Stop();
	void Async_Run();
};
} // namespace LittleEngine
