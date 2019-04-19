#pragma once
#include "CoreTypes.h"
#include "IService.h"

namespace LittleEngine
{
class Services
{
private:
	static Vec<IService*> s_pServices;
	static class EngineService* s_pEngine;
	static class RenderFactory* s_pRenderFactory;
	static class JobManager* s_pJobManager;
	static class GameManager* s_pGameManager;

public:
	static void Clear();

	template <typename T>
	static T* Locate();

	static EngineService* Engine();
	static RenderFactory* RFactory();
	static JobManager* Jobs();
	static GameManager* Game();
	static class Camera* WorldCamera();

	static void Provide(IService& service);
	static void Unprovide(IService& service);

private:
	static void ProvideEngine(EngineService& engine);
	static void UnprovideEngine(EngineService& engine);
	static void ProvideRenderFactory(RenderFactory& renderFactory);
	static void UnprovideRenderFactory(RenderFactory& renderFactory);
	static void ProvideJobManager(JobManager& jobManager);
	static void UnprovideJobManager(JobManager& jobManager);
	static void ProvideGameManager(GameManager& gameManager);
	static void UnprovideGameManager(GameManager& gameManager);

	friend class GameManager;
	friend class JobManager;
	friend class EngineService;
	friend class RenderHeap;
	friend class RenderFactory;
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
