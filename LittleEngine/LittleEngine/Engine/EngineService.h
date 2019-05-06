#pragma once
#include "Core/CoreTypes.h"
#include "SFMLAPI/Windowing/SFWindowData.h"
#include "LittleEngine/Services/IService.h"

namespace LittleEngine
{
class EngineService final : public IService
{
private:
	UPtr<class EngineInput> m_uEngineInput;
	UPtr<class EngineRepository> m_uEngineRepository;
	UPtr<class EngineAudio> m_uEngineAudio;
	UPtr<class WorldStateMachine> m_uWorldStateMachine;
	UPtr<class AsyncFileLogger> m_uFileLogger;
	class EngineLoop* m_pEngineLoop = nullptr;
	class AsyncRenderLoop* m_pRenderLoop = nullptr;
	bool m_bTerminating = false;

public:
	EngineService(EngineLoop& engineLoop);
	~EngineService() override;

	WorldStateMachine* Worlds() const;
	EngineRepository* Repository() const;
	EngineInput* Input() const;
	EngineAudio* Audio() const;

	void TrySetWindowSize(u32 height);
	void SetWindowStyle(SFWindowStyle newStyle);
	void Terminate();

private:
	void PreRun();
	void UpdateInput(const struct SFInputDataFrame& inputDataFrame);
	// Returns true if active World state changed or terminating
	bool Tick(Time dt);
	void PreFinishFrame();

	friend class EngineLoop;
};
} // namespace LittleEngine
