#include "stdafx.h"
#include "Services.h"
#include "Utils.h"

namespace LittleEngine
{
Vector<IService*> Services::s_pServices;
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
	LogI("[Services] [EngineService] Provided");
	s_pEngine = &engine;
	Provide(engine);
}

void Services::UnprovideEngine(EngineService& engine)
{
	Unprovide(engine);
	LogI("[Services] [EngineService] Unprovided");
	s_pEngine = nullptr;
}

void Services::ProvideRenderHeap(RenderHeap& renderHeap)
{
	LogI("[Services] [RenderHeap] Service Provided");
	Provide(renderHeap);
	s_pRenderHeap = &renderHeap;
}

void Services::UnprovideRenderHeap(RenderHeap& renderHeap)
{
	Unprovide(renderHeap);
	LogI("[Services] [RenderHeap] Service Unprovided");
	s_pRenderHeap = nullptr;
}

void Services::ProvideJobManager(JobManager& jobManager)
{
	LogI("[Services] [JobManager] Service Provided");
	Provide(jobManager);
	s_pJobManager = &jobManager;
}

void Services::UnprovideJobManager(JobManager& jobManager)
{
	Unprovide(jobManager);
	LogI("[Services] [JobManager] Service Unprovided");
	s_pJobManager = nullptr;
}

void Services::ProvideGameManager(GameManager& gameManager)
{
	LogI("[Services] " + gameManager.LogName() + " Service Provided");
	Provide(gameManager);
	s_pGameManager = &gameManager;
}

void Services::UnprovideGameManager(GameManager& gameManager)
{
	Unprovide(gameManager);
	LogI("[Services] " + gameManager.LogName() + " Service Unprovided");
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
