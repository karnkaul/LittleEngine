#pragma once
#include "CoreTypes.h"
#include "LittleEngine/Services/IService.h"
#include "LittleEngine/Input/EngineInput.h"
#include "LittleEngine/Audio/EngineAudio.h"
#include "LittleEngine/Game/World/WorldStateMachine.h"
#include "LittleEngine/Repository/EngineRepository.h"

namespace LittleEngine
{
class EngineService final : public IService
{
private:
	UPtr<EngineInput> m_uEngineInput;
	UPtr<EngineRepository> m_uAssetRepository;
	UPtr<EngineAudio> m_uEngineAudio;
	UPtr<WorldStateMachine> m_uWorldStateMachine;
	UPtr<class AsyncFileLogger> m_uFileLogger;
	bool m_bTerminate;

public:
	EngineService();
	~EngineService();

	WorldStateMachine* Worlds() const;
	EngineRepository* Repository() const;
	EngineInput* Input() const;
	EngineAudio* Audio() const;

	void Terminate();

private:
	void PreRun();
	void UpdateInput(const struct SFInputDataFrame& inputDataFrame);
	void Tick(Time dt);
	void PostTick();
	void PostBufferSwap();

	friend class EngineLoop;
	friend class EngineLoop;
};
} // namespace LittleEngine
