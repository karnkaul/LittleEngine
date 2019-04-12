#pragma once
#include "SFMLAPI/Rendering/GFXBuffer.h"
#include "SFMLAPI/Windowing/SFEventLoop.h"

namespace LittleEngine
{
// \brief: Core Game Loop that also manages an Asynchronous Render Loop
class EngineLoop final : public ASFEventLoop
{
private:
	GFXBuffer m_gfxBuffer;
	Vector2 m_cullBounds;
	UPtr<class EngineConfig> m_uConfig;
	UPtr<class JobManager> m_uJobManager;
	UPtr<class RenderHeap> m_uRenderHeap;
	UPtr<class AsyncRenderLoop> m_uAsyncRenderLoop;
	UPtr<class EngineService> m_uEngineService;
	bool m_bRenderThread = true;
	bool m_bInit = false;

public:
	static UPtr<EngineLoop> Create();

public:
	~EngineLoop() override;

	void Start();
	void StopTicking();

private:
	void PreRun() override;
	void Tick(Time dt) override;
	void PostTick() override;
	void PostRun() override;
	void OnPause(bool bPause) override;

	void UpdateInput();
	void Integrate(Time dt);
	void RenderCleanup();

private:
	void ReconcileRenderStates();
	void SwapGFXBuffer();
	void Init();
	void Uninit();

	friend class EngineService;
};
} // namespace LittleEngine
