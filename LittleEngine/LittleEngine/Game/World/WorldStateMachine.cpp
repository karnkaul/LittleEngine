#include "stdafx.h"
#include "WorldStateMachine.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/RenderLoop/RenderHeap.h"
#include "LittleEngine/Repository/AsyncAssetLoader.h"

namespace LittleEngine
{
namespace
{
Time loadTime;
}

bool WorldStateMachine::s_bReady;

WorldStateMachine::WorldStateMachine()
{
	s_bReady = false;
	LOG_I("[WorldStateMachine] constructed");
}

WorldStateMachine::~WorldStateMachine()
{
	if (m_pActiveState)
		m_pActiveState->Deactivate();
	m_uCreatedStates.clear();
	LOG_I("[WorldStateMachine] destroyed");
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

void WorldStateMachine::Start(const String& manifestPath)
{
	m_bLoading = true;
	// LoadAsync all assets in manifest
	if (!manifestPath.empty())
	{
		LOG_D("[WorldStateMachine] Loading assets from manifest [%s]...", manifestPath.c_str());
		m_pAssetLoader = Services::Engine()->Repository()->LoadAsync(manifestPath, [&]() {
			m_bLoaded = true;
			m_pAssetLoader = nullptr;
		});
		loadTime = Time::Zero;
		m_uLoadingUI = MakeUnique<LoadingUI>();
	}
	else
	{
		m_bLoaded = true;
	}
	// When done, load World 0
}

void WorldStateMachine::Tick(Time dt)
{
	if (m_bLoading)
	{
		LoadingTick(dt);
		return;
	}
	GameTick(dt);
}

void WorldStateMachine::PostBufferSwap()
{
	if (m_bToActivateState)
	{
		m_pActiveState->Activate();
		m_bToActivateState = false;
		LOG_I("[WorldStateMachine] ...Loaded %s", m_pNextState->LogNameStr());
	}
}

void WorldStateMachine::LoadingTick(Time dt)
{
	if (m_uLoadingUI)
	{
		Fixed progress = m_pAssetLoader ? m_pAssetLoader->GetProgress() : 1.0f;
		m_uLoadingUI->Tick(dt, progress);
	}
	if (m_bLoaded)
	{
		loadTime = Time::Now() - loadTime;
		LOG_I("[WorldStateMachine] ...Manifest load complete in %.2fs. Loading World 0", loadTime.AsSeconds());
		m_bLoading = false;
		m_uLoadingUI = nullptr;
		LoadState(0);
		s_bReady = true;
	}
}

void WorldStateMachine::GameTick(Time dt)
{
	if (m_pActiveState)
		m_pActiveState->Tick(dt);

	if (m_pNextState != m_pActiveState)
	{
		LOG_D("[WorldStateMachine] Loading %s...", m_pNextState->LogNameStr());
		if (m_pActiveState)
		{
			m_pActiveState->Deactivate();
		}
		m_pActiveState = m_pNextState;
		m_bToActivateState = true;
		Services::RHeap()->Reconcile();
	}
}
} // namespace LittleEngine