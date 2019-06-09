#include "stdafx.h"
#include "Core/Jobs.h"
#include "Core/Logger.h"
#include "Core/Utils.h"
#include "LittleEngine/Context/LEContext.h"
#include "LittleEngine/Physics/CollisionManager.h"
#include "LittleEngine/OS.h"
#include "GameManager.h"
#include "World/Component.h"
#include "World/Entity.h"
#include "World/Camera.h"
#include "LEGame/Model/UI/UIManager.h"
#include "LEGame/Model/World/WorldStateMachine.h"

namespace LittleEngine
{
using Version = Core::Version;
GameManager* g_pGameManager = nullptr;

namespace
{
Version gameVersion = "0.1";
}

const Version& GameManager::GetGameVersion()
{
	Version fileGameVersion = OS::Env()->GetFileGameVersion();
	if (fileGameVersion != Version())
	{
		gameVersion = fileGameVersion;
	}
	return gameVersion;
}

GameManager::GameManager(WorldStateMachine& wsm) : m_logName("[GameManager]"), m_pWSM(&wsm)
{
	m_uUIManager = MakeUnique<UIManager>();
	m_uCollisionManager = MakeUnique<CollisionManager>();
	m_uWorldCamera = MakeUnique<Camera>();
	m_uWorldCamera->SetName("WorldCamera");
	g_pGameManager = this;
	LOG_D("%s Created", m_logName.c_str());
}

GameManager::~GameManager()
{
	for (auto& componentVec : m_uComponents)
	{
		componentVec.clear();
	}
	m_uCollisionManager = nullptr;
	m_uEntities.clear();
	m_uUIManager = nullptr;
	m_uWorldCamera = nullptr;
	g_pGameManager = nullptr;
	LOG_D("%s Destroyed", m_logName.c_str());
}

UIManager* GameManager::UI() const
{
	return m_uUIManager.get();
}

LEInput* GameManager::Input() const
{
	Assert(m_pWSM && m_pWSM->m_pContext, "WSM/LEContext is null!");
	return m_pWSM->m_pContext->Input();
}

LERenderer* GameManager::Renderer() const
{
	Assert(m_pWSM && m_pWSM->m_pContext, "WSM/LEContext is null!");
	return m_pWSM->m_pContext->Renderer();
}

LEContext* GameManager::Context() const
{
	Assert(m_pWSM && m_pWSM->m_pContext, "WSM/LEContext is null!");
	return m_pWSM->m_pContext;
}

CollisionManager* GameManager::Physics() const
{
	return m_uCollisionManager.get();
}

bool GameManager::LoadWorld(WorldID id)
{
	return m_pWSM->LoadState(id);
}

WorldID GameManager::GetActiveWorldID() const
{
	return m_pWSM->GetActiveStateID();
}

Vec<WorldID> GameManager::GetAllWorldIDs() const
{
	return m_pWSM->GetAllStateIDs();
}

void GameManager::Quit()
{
	if (!Core::Jobs::AreWorkersIdle())
	{
		Assert(false, "Quit() called while job workers are active!");
		LOG_E("%s Quit() called while job workers are active!", m_logName.c_str());
		m_bWaitingToTerminate = true;
	}
	else
	{
		m_pWSM->m_pContext->Terminate();
	}
}

Camera* GameManager::WorldCamera() const
{
	return m_uWorldCamera.get();
}

void GameManager::SetWorldCamera(UPtr<Camera> uCamera)
{
	m_uWorldCamera = std::move(uCamera);
}

const char* GameManager::LogNameStr() const
{
	return m_logName.c_str();
}

void GameManager::Tick(Time dt)
{
	m_uCollisionManager->Tick(dt);
	for (auto& componentVec : m_uComponents)
	{
		Core::RemoveIf<UPtr<AComponent>>(componentVec,
										   [](UPtr<AComponent>& uC) { return uC->m_bDestroyed; });
		for (auto& uComponent : componentVec)
		{
			uComponent->Tick(dt);
		}
	}
	Core::RemoveIf<UPtr<Entity>>(m_uEntities, [](UPtr<Entity>& uE) { return uE->m_bDestroyed; });
	for (auto& uEntity : m_uEntities)
	{
		uEntity->Tick(dt);
	}
	m_uUIManager->Tick(dt);
	m_uWorldCamera->Tick(dt);
}
} // namespace LittleEngine
