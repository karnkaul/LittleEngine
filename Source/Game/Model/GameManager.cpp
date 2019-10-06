#include "Core/Game/Jobs.h"
#include "Core/Game/OS.h"
#include "Core/Game/LECoreUtils/Utils.h"
#include "Engine/Context/LEContext.h"
#include "Engine/Debug/Tweakable.h"
#include "Engine/Locale/Locale.h"
#include "Engine/Physics/LEPhysics.h"
#include "GameManager.h"
#include "World/Component.h"
#include "World/Entity.h"
#include "World/Camera.h"
#include "Model/GameConfig.h"
#include "Model/GameKernel.h"
#include "Model/GameSettings.h"
#include "Model/UI/UIManager.h"
#include "Model/World/WorldStateMachine.h"

namespace LittleEngine
{
GameManager* g_pGameManager = nullptr;
TweakBool(paused, nullptr);

GameManager::GameManager(LEContext& context, WorldStateMachine& wsm) : m_logName("[GameManager]"), m_pContext(&context), m_pWSM(&wsm)
{
	Assert(&wsm, "WSM is null!");
	Assert(&context, "WSM is null!");
	g_pGameManager = this;
	m_uUIManager = std::make_unique<UIManager>();
	m_uWorldCamera = std::make_unique<Camera>();
	m_uWorldCamera->SetName("WorldCamera");
	m_uPhysics = std::make_unique<LEPhysics>();
#if ENABLED(TWEAKABLES)
	paused.Bind(&m_bPaused);
#endif
	LOG_D("%s Created", m_logName.c_str());
}

GameManager::~GameManager()
{
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
	Assert(m_pContext, "WSM is null!");
	return m_pContext->Input();
}

LERenderer* GameManager::Renderer() const
{
	Assert(m_pContext, "WSM is null!");
	return m_pContext->Renderer();
}

LEContext* GameManager::Context() const
{
	Assert(m_pContext, "WSM is null!");
	return m_pContext;
}

LEPhysics* GameManager::Physics() const
{
	return m_uPhysics.get();
}

void GameManager::ReloadWorld()
{
	m_pWSM->LoadWorld(m_pWSM->ActiveWorldID());
}

bool GameManager::LoadWorld(WorldID id)
{
	return m_pWSM->LoadWorld(id);
}

WorldID GameManager::ActiveWorldID() const
{
	return m_pWSM->ActiveWorldID();
}

std::vector<WorldID> GameManager::AllWorldIDs() const
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

void GameManager::OnWorldUnloaded()
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

void GameManager::Tick(Time dt)
{
	if (m_pWSM->Tick(dt) == WorldStateMachine::State::Running)
	{
		if (m_bQuitting)
		{
			m_pWSM->Quit();
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
