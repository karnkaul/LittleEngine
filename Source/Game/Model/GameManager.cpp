#include "Core/Jobs.h"
#include "Core/Logger.h"
#include "Core/OS.h"
#include "Core/Utils.h"
#include "Engine/Context/LEContext.h"
#include "Engine/Debug/Tweakable.h"
#include "Engine/Locale/Locale.h"
#include "Engine/Physics/LEPhysics.h"
#include "GameManager.h"
#include "World/Component.h"
#include "World/Entity.h"
#include "World/Camera.h"
#include "Model/GameConfig.h"
#include "Model/GameSettings.h"
#include "Model/UI/UIManager.h"
#include "Model/World/WorldStateMachine.h"

namespace LittleEngine
{
GameManager* g_pGameManager = nullptr;
TweakBool(paused, nullptr);

GameManager::GameManager(LEContext& context) : m_logName("[GameManager]"), m_pContext(&context)
{
	g_pGameManager = this;
	m_uWSM = MakeUnique<WorldStateMachine>(*m_pContext);
	m_uUIManager = MakeUnique<UIManager>();
	m_uWorldCamera = MakeUnique<Camera>();
	m_uWorldCamera->SetName("WorldCamera");
	m_uPhysics = MakeUnique<LEPhysics>();
#if ENABLED(TWEAKABLES)
	paused.Bind(&m_bPaused);
#endif
	LOG_D("%s Created", m_logName.c_str());
}

GameManager::~GameManager()
{
	m_uWSM = nullptr;
	m_entities.clear();
	for (auto& vec : m_components)
	{
		vec.clear();
	}
	m_uPhysics = nullptr;
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
	return m_pContext->Input();
}

LERenderer* GameManager::Renderer() const
{
	return m_pContext->Renderer();
}

LEContext* GameManager::Context() const
{
	return m_pContext;
}

LEPhysics* GameManager::Physics() const
{
	return m_uPhysics.get();
}

bool GameManager::LoadWorld(WorldID id)
{
	return m_uWSM->LoadWorld(id);
}

WorldID GameManager::ActiveWorldID() const
{
	return m_uWSM->ActiveWorldID();
}

Vec<WorldID> GameManager::AllWorldIDs() const
{
	return m_uWSM->AllWorldIDs();
}

void GameManager::Start(String coreManifestID /* = "" */, String gameStyleID /* = "" */, Task onManifestLoaded /* = nullptr */)
{
	m_uWSM->Start(std::move(coreManifestID), std::move(gameStyleID), std::move(onManifestLoaded));
}

void GameManager::SetPaused(bool bPaused)
{
	m_bPaused = bPaused;
}

void GameManager::Quit()
{
	m_bQuitting = true;
}

void GameManager::Clear()
{
	for (auto& componentVec : m_components)
	{
		componentVec.clear();
	}
	m_entities.clear();
	m_uUIManager->Clear();
	m_uPhysics->Clear();
	m_uWorldCamera->Reset();
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

#ifdef DEBUGGING
void GameManager::ModifyTickRate(Time newTickRate)
{
	m_pContext->ModifyTickRate(newTickRate);
}
#endif

void GameManager::Tick(Time dt)
{
	m_uWSM->Tick(dt);
	if (m_uWSM->m_state == WorldStateMachine::State::Running)
	{
		if (m_bQuitting)
		{
			m_uWSM->Quit();
			return;
		}
		if (!m_bPaused)
		{
			for (auto& componentVec : m_components)
			{
				Core::RemoveIf<UPtr<AComponent>>(componentVec, [](UPtr<AComponent>& uC) { return uC->m_bDestroyed; });
				for (auto& uComponent : componentVec)
				{
					uComponent->Tick(dt);
				}
			}
			Core::RemoveIf<UPtr<Entity>>(m_entities, [](UPtr<Entity>& uE) { return uE->IsDestroyed(); });
			for (auto& uEntity : m_entities)
			{
				uEntity->Tick(dt);
			}
			m_uWorldCamera->Tick(dt);
		}
		m_uUIManager->Tick(dt);
	}
}

void GameManager::Step(Time fdt) 
{
	if (!m_bPaused)
	{
		m_uPhysics->Step(fdt);
		for (auto& componentVec : m_components)
		{
			for (auto& uComponent : componentVec)
			{
				uComponent->Step(fdt);
			}
		}
		for (auto& uEntity : m_entities)
		{
			uEntity->Step(fdt);
		}
	}
}
} // namespace LittleEngine
