#pragma once
#include "CoreTypes.h"
#include "IService.h"
#include "LittleEngine/Engine/EngineService.h"
#include "LittleEngine/RenderLoop/RenderHeap.h"
#include "LittleEngine/Jobs/JobManager.h"
#include "LittleEngine/Game/GameManager.h"

namespace LittleEngine
{
class Services
{
private:
	static Vec<IService*> s_pServices;
	static EngineService* s_pEngine;
	static RenderHeap* s_pRenderHeap;
	static JobManager* s_pJobManager;
	static GameManager* s_pGameManager;

public:
	static void Clear();

	template <typename T>
	static T* Locate();

	static EngineService* Engine();
	static RenderHeap* RHeap();
	static JobManager* Jobs();
	static GameManager* Game();

	static void Provide(IService& service);
	static void Unprovide(IService& service);

private:
	static void ProvideEngine(EngineService& engine);
	static void UnprovideEngine(EngineService& engine);
	static void ProvideRenderHeap(RenderHeap& renderHeap);
	static void UnprovideRenderHeap(RenderHeap& renderHeap);
	static void ProvideJobManager(JobManager& jobManager);
	static void UnprovideJobManager(JobManager& jobManager);
	static void ProvideGameManager(GameManager& gameManager);
	static void UnprovideGameManager(GameManager& gameManager);

	friend class GameManager;
	friend class JobManager;
	friend class EngineService;
	friend class RenderHeap;
};

template <typename T>
T* Services::Locate()
{
	for (auto pService : s_pServices)
	{
		T* pT = dynamic_cast<T*>(pService);
		if (pT)
			return pT;
	}
	return nullptr;
}
} // namespace LittleEngine
