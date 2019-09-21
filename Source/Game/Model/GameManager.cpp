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

GameManager::GameManager() : m_logName("[GameManager]")
{
	// TODO: Set global GFX struct

	g_pGameManager = this;
#if ENABLED(TWEAKABLES)
	paused.Bind(&m_bPaused);
#endif
	LOG_D("%s Created", m_logName.c_str());
}

GameManager::~GameManager()
{
	m_uWSM = nullptr;
	m_uCollisionManager = nullptr;
	m_uUIManager = nullptr;
	m_uWorldCamera = nullptr;
	m_uContext = nullptr;
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
	return m_uContext->Input();
}

LERenderer* GameManager::Renderer() const
{
	return m_uContext->Renderer();
}

LEContext* GameManager::Context() const
{
	return m_uContext.get();
}

LEPhysics* GameManager::Physics() const
{
	return m_uCollisionManager.get();
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

void GameManager::CreateContext(const GameConfig& config)
{
	GameSettings& settings = *GameSettings::Instance();
	m_gfx.m_uiSpace = config.UISpace();
	m_gfx.m_viewportHeight = ToS32(settings.ViewportHeight());
	m_gfx.SetWorldHeight(config.WorldHeight(), true);
#ifdef DEBUGGING
	m_gfx.m_overrideNativeAR = config.ForceViewportAR();
#endif
	m_gfx.Init();

	LEContextData data;
	data.viewportData.viewportSize = settings.SafeGetViewportSize();
	data.viewportData.title = LOC(config.TitleBarText());
	data.viewportData.style = settings.GetViewportStyle();
	data.tickRate = config.TickRate();
	data.bRenderThread = config.m_bRenderThread;
	data.renderThreadStartDelay = config.RenderThreadStartDelay();
	data.bPauseOnFocusLoss = config.ShouldPauseOnFocusLoss();
	Core::Property::Persistor inputMapPersistor;
	auto pInputMapFile = settings.GetValue("CUSTOM_INPUT_MAP");
	if (pInputMapFile)
	{
		String inputMapFile = OS::Env()->FullPath(pInputMapFile->c_str());
		if (inputMapPersistor.Load(inputMapFile))
		{
			u16 count = data.inputMap.Import(inputMapPersistor);
			if (count > 0)
			{
				LOG_I("[GameLoop] Loaded %u custom Input Mappings successfully", count);
			}
		}
	}

	m_uContext = MakeUnique<LEContext>(std::move(data));
	m_uWSM = MakeUnique<WorldStateMachine>(*m_uContext);
	m_uUIManager = MakeUnique<UIManager>();
	m_uWorldCamera = MakeUnique<Camera>();
	m_uWorldCamera->SetName("WorldCamera");
	m_uCollisionManager = MakeUnique<LEPhysics>();
}

#ifdef DEBUGGING
void GameManager::ModifyTickRate(Time newTickRate)
{
	m_uContext->ModifyTickRate(newTickRate);
}
#endif

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

void GameManager::Tick(Time dt, bool& bYieldIntegration)
{
	m_uWSM->Tick(dt, bYieldIntegration);
	if (m_uWSM->m_state == WorldStateMachine::State::Running)
	{
		if (m_bQuitting)
		{
			m_uWSM->Quit();
			Reset();
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
	bYieldIntegration |= m_uContext->Update();
}
} // namespace LittleEngine
