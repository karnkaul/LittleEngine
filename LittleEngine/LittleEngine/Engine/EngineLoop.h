#pragma once
#include "SFMLAPI/Windowing/SFEventLoop.h"

namespace LittleEngine
{
// \brief: Core Game Loop that also manages an Asynchronous Render Loop
class EngineLoop final : public ASFEventLoop
{
private:
	Vector2 m_cullBounds;
	UPtr<class EngineConfig> m_uConfig;
	UPtr<class JobManager> m_uJobManager;
	UPtr<class RenderFactory> m_uRenderFactory;
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
	void PostTicks() override;
	void PostRun() override;
	void OnPause(bool bPause) override;

	void UpdateInput();
	void Integrate(Time dt);
	void RenderCleanup();

private:
	void ReconcileGameStates();
	void SwapGameStates();
	void Init();
	void Uninit();

	friend class EngineService;
};
} // namespace LittleEngine
