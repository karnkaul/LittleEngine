#include "stdafx.h"
#include "Core/Utils.h"
#include "LittleEngine/Jobs/JobManager.h"
#include "LittleEngine/Engine/EngineService.h"
#include "LittleEngine/RenderLoop/RenderFactory.h"
#include "LittleEngine/Game/Camera.h"
#include "LittleEngine/Game/GameManager.h"
#include "LittleEngine/Game/World/WorldStateMachine.h"
#include "Services.h"

namespace LittleEngine
{
Vec<IService*> Services::s_pServices;
EngineService* Services::s_pEngine = nullptr;
RenderFactory* Services::s_pRenderFactory = nullptr;
JobManager* Services::s_pJobManager = nullptr;
GameManager* Services::s_pGameManager = nullptr;

EngineService* Services::Engine()
{
	Assert(s_pEngine, "Services: [EngineService] is null!");
	return s_pEngine;
}

RenderFactory* Services::RFactory()
{
	Assert(s_pRenderFactory, "Services: [RenderHeap] is null!");
	return s_pRenderFactory;
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


Camera* Services::WorldCamera()
{
	return s_pGameManager ? s_pGameManager->WorldCamera() : nullptr;
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

void Services::ProvideRenderFactory(RenderFactory& renderFactory)
{
	LOG_I("[Services] [RenderFactory] Service provided");
	Provide(renderFactory);
	s_pRenderFactory = &renderFactory;
}


void Services::UnprovideRenderFactory(RenderFactory& renderFactory)
{
	Unprovide(renderFactory);
	LOG_I("[Services] [RenderFactory] Service Unprovided");
	s_pRenderFactory = nullptr;
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
	LOG_D("[Services] %s Service Provided", gameManager.LogNameStr());
	Provide(gameManager);
	s_pGameManager = &gameManager;
}

void Services::UnprovideGameManager(GameManager& gameManager)
{
	Unprovide(gameManager);
	LOG_D("[Services] %s Service Unprovided", gameManager.LogNameStr());
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
