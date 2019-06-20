#include "stdafx.h"
#include "Core/ArchiveReader.h"
#include "Core/Logger.h"
#include "SFMLAPI/System/SFAssets.h"
#include "WSMLoadingUI.h"
#include "WSMErrorUI.h"
#include "WorldStateMachine.h"
#include "LittleEngine/Context/LEContext.h"
#include "LittleEngine/Repository/LERepository.h"
#include "LittleEngine/Repository/ManifestLoader.h"
#include "LEGame/Model/GameManager.h"
#include "LEGame/Model/UI/UIGameStyle.h"
#include "LEGame/Gameplay/GameInit.h"

namespace LittleEngine
{
namespace
{
Time loadTime;
}

bool WorldStateMachine::s_bReady = false;
bool WorldStateMachine::s_bHasState = false;
Vec<UPtr<World>> WorldStateMachine::s_uCreatedStates;
#if DEBUGGING
bool WorldStateMachine::s_bTEST_infiniteLoad = false;
#endif

WorldStateMachine::WorldStateMachine(LEContext& context) : m_pContext(&context)
{
	for (auto& uState : s_uCreatedStates)
	{
		uState->m_pWSM = this;
	}
	LOG_I("[WSM] Processed %u created Worlds", s_uCreatedStates.size());
}

WorldStateMachine::~WorldStateMachine()
{
	if (m_pActiveState)
	{
		m_pActiveState->Deactivate();
	}
	s_uCreatedStates.clear();
	LOG_D("[WSM] destroyed");
}

World* WorldStateMachine::GetActiveState() const
{
	return m_pActiveState;
}

WorldID WorldStateMachine::GetActiveStateID() const
{
	return m_pActiveState ? m_pActiveState->m_id : -1;
}

Vec<WorldID> WorldStateMachine::GetAllStateIDs() const
{
	Vec<WorldID> ret;
	for (const auto& uState : s_uCreatedStates)
	{
		ret.push_back(uState->m_id);
	}
	return ret;
}

bool WorldStateMachine::LoadState(WorldID id)
{
	for (auto& uState : s_uCreatedStates)
	{
		if (uState->m_id == id)
		{
			m_pNextState = uState.get();
			LOG_D("[WSM] Load Enqueued: %s", m_pNextState->LogNameStr());
			return true;
		}
	}
	LOG_E("[WSM] State ID [%d] does not exist!", id);
	return false;
}

void WorldStateMachine::Start(String manifestPath, String gameStyleID)
{
	m_manifestPath = std::move(manifestPath);
	m_bLoading = true;
	if (!m_manifestPath.empty() && s_bHasState)
	{
		// LoadAsync all assets in manifest
		m_pAssetLoader = g_pRepository->LoadAsync(m_manifestPath, [&, gameStyleID]() {
			if (!gameStyleID.empty())
			{
				auto pText = g_pRepository->Load<TextAsset>(gameStyleID);
				if (pText)
				{
					UIGameStyle::Load(pText->GetText());
				}
			}
			m_bLoaded = true;
			m_pAssetLoader = nullptr;
		});
		loadTime = Time::Now();
		if (!m_uLoadingUI)
		{
			m_uLoadingUI = MakeUnique<WSMLoadingUI>(*m_pContext);
		}
	}
	else
	{
		m_bLoaded = true;
	}
	// When done, load World 0
}

bool WorldStateMachine::Tick(Time dt)
{
	if (m_bLoading)
	{
		LoadingTick(dt);
		return false;
	}
	return GameTick(dt);
}

void WorldStateMachine::LoadingTick(Time dt)
{
	if (m_uLoadingUI)
	{
		Fixed progress = m_pAssetLoader ? m_pAssetLoader->GetProgress() : Fixed::One;
		m_uLoadingUI->Tick(dt, progress);
	}
	if (m_bLoaded)
	{
		GameInit::LoadShaders();
		auto size = Core::GetFriendlySize(g_pRepository->GetLoadedBytes());
		loadTime = Time::Now() - loadTime;
		LOG_I("[WSM] Manifest load completed in %.2fs. Repository size: %.2f%s", loadTime.AsSeconds(), size.first, size.second);
#if DEBUGGING
		if (s_bTEST_infiniteLoad)
		{
			g_pRepository->UnloadAll(false);
			m_bLoaded = false;
			m_uLoadingUI->Reset();
			LOG_D("[WSM] Reloading all assets");
			Start(m_manifestPath);
			return;
		}
#endif
		m_bLoading = false;
		m_uLoadingUI = nullptr;
		if (s_bHasState)
		{
			LOG_D("[WSM] Loading World 0...");
			LoadState(0);
		}
		else
		{
			LOG_E(
				"[WSM] No Worlds created! Call "
				"`WorldStateMachine::CreateWorlds<T...>()` before `GameLoop::Run()`");
			m_uErrorUI = MakeUnique<WSMErrorUI>(*m_pContext);
			m_uErrorUI->SetTitle("No Worlds Created!");
			m_uErrorUI->SetDescription(
				"Before `GameLoop::Run()`, call \n"
				"`WorldStateMachine::CreateWorlds<T...>()`");
		}
		s_bReady = true;
	}
}

bool WorldStateMachine::GameTick(Time dt)
{
	bool bYield = false;
	if (m_bToActivateState && m_pActiveState)
	{
		m_pActiveState->Activate();
		m_bToActivateState = false;
		bYield = true;
		LOG_I("[WSM] Loaded %s", m_pActiveState->LogNameStr());
	}

	if (m_pActiveState)
	{
		m_pActiveState->Tick(dt);
	}

	if (m_pNextState && (m_pNextState != m_pActiveState))
	{
		LOG_D("[WSM] Loading %s...", m_pNextState->LogNameStr());
		if (m_pActiveState)
		{
			m_pActiveState->Deactivate();
		}
		m_pActiveState = m_pNextState;
		m_bToActivateState = true;
	}
	return bYield | (m_pActiveState == nullptr);
}

void WorldStateMachine::Quit()
{
	if (m_pActiveState)
	{
		m_pActiveState->Deactivate();
	}
	m_pNextState = m_pActiveState = nullptr;
	m_pContext->Terminate();
}
} // namespace LittleEngine
