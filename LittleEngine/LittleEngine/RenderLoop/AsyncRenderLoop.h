#pragma once
#include "CoreTypes.h"
#include "LittleEngine/Jobs/JobManager.h"
#include "SFMLAPI/SFSystem.h"
#include "SFMLAPI/Rendering/SFRenderer.h"

namespace LittleEngine
{
// \brief: Starts render loop on another thread on construction, joins thread on destruction.
// \warning: Ensure GLWindow remains alive until destruction is complete!
class AsyncRenderLoop final : public SFRenderer
{
private:
	JobHandle m_renderJobHandle = -1;
	GFXBuffer* m_pBuffer;

public:
	AsyncRenderLoop(SFWindow& glWindow, class GFXBuffer& gfxBuffer, Time tickRate, bool bStartThread);
	~AsyncRenderLoop();

private:
	void Run();
};
} // namespace LittleEngine
