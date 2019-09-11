#include "Core/Jobs.h"
#include "Core/Logger.h"
#include "Core/Utils.h"
#include "Engine/Context/LEContext.h"
#include "Engine/Debug/Tweakable.h"
#include "Engine/Physics/LEPhysics.h"
#include "GameManager.h"
#include "World/Component.h"
#include "World/Entity.h"
#include "World/Camera.h"
#include "Model/UI/UIManager.h"
#include "Model/World/WorldStateMachine.h"

namespace LittleEngine
{
GameManager* g_pGameManager = nullptr;
TweakBool(paused, nullptr);

GameManager::GameManager(WorldStateMachine& wsm) : m_logName("[GameManager]"), m_pWSM(&wsm)
{
	m_uUIManager = MakeUnique<UIManager>();

	m_uWorldCamera = MakeUnique<Camera>();
	m_uWorldCamera->SetName("WorldCamera");

	m_uCollisionManager = MakeUnique<LEPhysics>();

	g_pGameManager = this;
#if ENABLED(TWEAKABLES)
	paused.Bind(&m_bPaused);
#endif
	LOG_D("%s Created", m_logName.c_str());
}

GameManager::~GameManager()
{
	Reset();
	m_uCollisionManager = nullptr;
	m_uUIManager = nullptr;
	m_uWorldCamera = nullptr;
	g_pGameManager = nullptr;
#if ENABLED(TWEAKABLES)
	paused.Bind(nullptr);
#endif
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

LEPhysics* GameManager::Physics() const
{
	return m_uCollisionManager.get();
}

bool GameManager::LoadWorld(WorldID id)
{
	return m_pWSM->LoadWorld(id);
}

WorldID GameManager::ActiveWorldID() const
{
	return m_pWSM->ActiveWorldID();
}

Vec<WorldID> GameManager::AllWorldIDs() const
{
	return m_pWSM->AllWorldIDs();
}

void GameManager::SetPaused(bool bPaused)
{
	m_bPaused = bPaused;
}

void GameManager::Quit()
{
	m_bQuitting = true;
}

Camera* GameManager::WorldCamera() const
{
	return m_uWorldCamera.get();
}

void GameManager::SetWorldCamera(UPtr<Camera> uCamera)
{
	m_uWorldCamera = std::move(uCamera);
}

bool GameManager::IsPaused() const
{
	return m_bPaused;
}

bool GameManager::IsPlayerControllable() const
{
	return !m_bPaused && !(UI()->Active());
}

const char* GameManager::LogNameStr() const
{
	return m_logName.c_str();
}

void GameManager::Reset()
{
	for (auto& componentVec : m_uComponents)
	{
		componentVec.clear();
	}
	m_uEntities.clear();
	m_uUIManager->Reset();
	m_uWorldCamera->Reset();
	m_uCollisionManager->Reset();
}

void GameManager::Tick(Time dt)
{
	if (m_bQuitting)
	{
		m_pWSM->Quit();
		return;
	}

	if (!m_bPaused)
	{
		m_uCollisionManager->Tick(dt);
		for (auto& componentVec : m_uComponents)
		{
			Core::RemoveIf<UPtr<AComponent>>(componentVec, [](UPtr<AComponent>& uC) { return uC->m_bDestroyed; });
			for (auto& uComponent : componentVec)
			{
				uComponent->Tick(dt);
			}
		}
		Core::RemoveIf<UPtr<Entity>>(m_uEntities, [](UPtr<Entity>& uE) { return uE->IsDestroyed(); });
		for (auto& uEntity : m_uEntities)
		{
			uEntity->Tick(dt);
		}
		m_uWorldCamera->Tick(dt);
	}
	m_uUIManager->Tick(dt);
}
} // namespace LittleEngine
