#include "stdafx.h"
#include "Core/ArchiveReader.h"
#include "Core/Logger.h"
#include "SFMLAPI/System/SFAssets.h"
#include "LoadingUI.h"
#include "WorldStateMachine.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/Repository/EngineRepository.h"
#include "LittleEngine/Repository/ManifestLoader.h"
#include "LittleEngine/Engine/EngineService.h"
#include "LittleEngine/UI/UIGameStyle.h"

namespace LittleEngine
{
namespace
{
Time loadTime;
}

bool WorldStateMachine::s_bReady;
#if DEBUGGING
bool WorldStateMachine::s_bTEST_infiniteLoad = false;
#endif

WorldStateMachine::WorldStateMachine()
{
	s_bReady = false;
	LOG_D("[WorldStateMachine] constructed");
}

WorldStateMachine::~WorldStateMachine()
{
	if (m_pActiveState)
	{
		m_pActiveState->Deactivate();
	}
	m_uCreatedStates.clear();
	LOG_D("[WorldStateMachine] destroyed");
}

World* WorldStateMachine::GetActiveState() const
{
	return m_pActiveState;
}

WorldID WorldStateMachine::GetActiveStateID() const
{
	return m_pActiveState ? m_pActiveState->m_id : -1;
}

bool WorldStateMachine::LoadState(WorldID id)
{
	for (auto& uState : m_uCreatedStates)
	{
		if (uState->m_id == id)
		{
			m_pNextState = uState.get();
			LOG_D("[WorldStateMachine] Load Enqueued: %s", m_pNextState->LogNameStr());
			return true;
		}
	}
	LOG_E("[WorldStateMachine] State ID [%d] does not exist!", id);
	return false;
}

void WorldStateMachine::Start(String manifestPath, String gameStyleID)
{
	m_manifestPath = std::move(manifestPath);
	m_bLoading = true;
	if (!m_manifestPath.empty())
	{
		// LoadAsync all assets in manifest
		m_pAssetLoader = Services::Engine()->Repository()->LoadAsync(m_manifestPath, [&, gameStyleID]() {
			if (!gameStyleID.empty())
			{
				TextAsset* pText = Services::Engine()->Repository()->Load<TextAsset>(gameStyleID);
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
			m_uLoadingUI = MakeUnique<LoadingUI>();
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
		loadTime = Time::Now() - loadTime;
		LOG_I("[WorldStateMachine] Manifest load complete in %.2fs. Loading World 0...", loadTime.AsSeconds());
#if DEBUGGING
		if (s_bTEST_infiniteLoad)
		{
			Services::Engine()->Repository()->UnloadAll(false);
			m_bLoaded = false;
			m_uLoadingUI->Reset();
			LOG_D("[WorldStateMachine] Reloading all assets");
			Start(m_manifestPath);
			return;
		}
#endif
		m_bLoading = false;
		m_uLoadingUI = nullptr;
		LoadState(0);
		s_bReady = true;
	}
}

bool WorldStateMachine::GameTick(Time dt)
{
	bool bYield = false;
	if (m_bToActivateState)
	{
		m_pActiveState->Activate();
		m_bToActivateState = false;
		bYield = true;
		LOG_I("[WorldStateMachine] ...Loaded %s", m_pNextState->LogNameStr());
	}

	if (m_pActiveState)
	{
		m_pActiveState->Tick(dt);
	}

	if (m_pNextState != m_pActiveState)
	{
		LOG_D("[WorldStateMachine] Loading %s...", m_pNextState->LogNameStr());
		if (m_pActiveState)
		{
			m_pActiveState->Deactivate();
		}
		m_pActiveState = m_pNextState;
		m_bToActivateState = true;
	}
	return bYield;
}

Vec<WorldID> WorldStateMachine::GetAllStateIDs() const
{
	Vec<WorldID> ret;
	for (const auto& uState : m_uCreatedStates)
	{
		ret.push_back(uState->m_id);
	}
	return ret;
}

} // namespace LittleEngine
