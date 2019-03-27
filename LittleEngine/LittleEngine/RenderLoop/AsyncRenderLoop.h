#pragma once
#include "CoreTypes.h"
#include "SFMLAPI/Rendering/SFRenderer.h"

namespace LittleEngine
{
// \brief: Starts render loop on another thread on construction, joins thread on destruction.
// \warning: Ensure GLWindow remains alive until destruction is complete!
class AsyncRenderLoop final : public SFRenderer
{
private:
	SPtr<class JobHandle> m_pRenderJobHandle;
	class GFXBuffer* m_pBuffer;

public:
	AsyncRenderLoop(SFWindow& sfWindow, GFXBuffer& gfxBuffer, Time tickRate, bool bStartThread);
	~AsyncRenderLoop();

private:
	void Run();
};
} // namespace LittleEngine
