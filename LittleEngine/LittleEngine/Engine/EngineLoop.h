#pragma once
#include "EngineConfig.h"
#include "LittleEngine/Jobs/JobManager.h"
#include "LittleEngine/RenderLoop/AsyncRenderLoop.h"
#include "SFMLAPI/SFWindowing.h"
#include "SFMLAPI/SFRendering.h"

namespace LittleEngine
{
// \brief: Core Game Loop that also manages an Asynchronous Render Loop
class EngineLoop final : public SFEventLoop
{
private:
	GFXBuffer m_gfxBuffer;
	EngineConfig m_config;
	UPtr<JobManager> m_uJobManager;
	UPtr<class RenderHeap> m_uRenderHeap;
	UPtr<AsyncRenderLoop> m_uAsyncRenderLoop;
	UPtr<class EngineService> m_uEngineService;
	bool m_bRenderThread;
	bool m_bInit = false;

public:
	static UPtr<EngineLoop> Create();

	void Start();
	void StopTicking();

protected:
	virtual void PreRun() override;
	virtual void Tick(Time dt) override;
	virtual void PostTick() override;
	virtual void PostRun() override;
	virtual void OnPause(bool bPause) override;

	void UpdateInput();
	void Integrate(Time dt);
	void RenderCleanup();

private:
	void ReconcileRenderStates();
	void SwapGFXBuffer();
	void Init();
	void Uninit();
};
} // namespace LittleEngine
