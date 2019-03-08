#include "stdafx.h"
#include "Services.h"
#include "Utils.h"
#include "LittleEngine/Jobs/JobManager.h"
#include "LittleEngine/Engine/EngineService.h"
#include "LittleEngine/RenderLoop/RenderHeap.h"
#include "LittleEngine/Game/GameManager.h"

namespace LittleEngine
{
Vec<IService*> Services::s_pServices;
EngineService* Services::s_pEngine = nullptr;
RenderHeap* Services::s_pRenderHeap = nullptr;
JobManager* Services::s_pJobManager = nullptr;
GameManager* Services::s_pGameManager = nullptr;

EngineService* Services::Engine()
{
	Assert(s_pEngine, "Services: [EngineService] is null!");
	return s_pEngine;
}

RenderHeap* Services::RHeap()
{
	Assert(s_pRenderHeap, "Services: [RenderHeap] is null!");
	return s_pRenderHeap;
}

JobManager* Services::Jobs()
{
	Assert(s_pJobManager, "Services: [JobManager] is null!");
	return s_pJobManager;
}

GameManager* Services::Game()
{
	Assert(s_pGameManager, "Services: [GameManager] is null!");
	return s_pGameManager;
}

void Services::ProvideEngine(EngineService& engine)
{
	LOG_I("[Services] [EngineService] Provided");
	s_pEngine = &engine;
	Provide(engine);
}

void Services::UnprovideEngine(EngineService& engine)
{
	Unprovide(engine);
	LOG_I("[Services] [EngineService] Unprovided");
	s_pEngine = nullptr;
}

void Services::ProvideRenderHeap(RenderHeap& renderHeap)
{
	LOG_I("[Services] [RenderHeap] Service Provided");
	Provide(renderHeap);
	s_pRenderHeap = &renderHeap;
}

void Services::UnprovideRenderHeap(RenderHeap& renderHeap)
{
	Unprovide(renderHeap);
	LOG_I("[Services] [RenderHeap] Service Unprovided");
	s_pRenderHeap = nullptr;
}

void Services::ProvideJobManager(JobManager& jobManager)
{
	LOG_I("[Services] [JobManager] Service Provided");
	Provide(jobManager);
	s_pJobManager = &jobManager;
}

void Services::UnprovideJobManager(JobManager& jobManager)
{
	Unprovide(jobManager);
	LOG_I("[Services] [JobManager] Service Unprovided");
	s_pJobManager = nullptr;
}

void Services::ProvideGameManager(GameManager& gameManager)
{
	LOG_I("[Services] %s Service Provided", gameManager.LogNameStr());
	Provide(gameManager);
	s_pGameManager = &gameManager;
}

void Services::UnprovideGameManager(GameManager& gameManager)
{
	Unprovide(gameManager);
	LOG_I("[Services] %s Service Unprovided", gameManager.LogNameStr());
	s_pGameManager = nullptr;
}

void Services::Provide(IService& service)
{
	s_pServices.push_back(&service);
}

void Services::Unprovide(IService& service)
{
	Core::VectorErase(s_pServices, &service);
}

void Services::Clear()
{
	s_pServices.clear();
}
} // namespace LittleEngine
