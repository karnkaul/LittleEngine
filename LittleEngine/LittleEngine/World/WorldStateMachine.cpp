#include "stdafx.h"
#include "WorldStateMachine.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/RenderLoop/RenderHeap.h"

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
	LogI("[WorldStateMachine] constructed");
}

WorldStateMachine::~WorldStateMachine()
{
	if (m_pActiveState)
		m_pActiveState->Deactivate();
	m_uCreatedStates.clear();
	LogI("[WorldStateMachine] destroyed");
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
			LogD("[WorldStateMachine] Load Enqueued: " + m_pNextState->LogName(*m_pNextState));
			return true;
		}
	}
	LogE("[WorldStateMachine] State ID " + Strings::ToString(id) + " does not exist!");
	return false;
}

void WorldStateMachine::Start(const String& manifestPath)
{
	m_bLoading = true;
	// LoadAsync all assets in manifest
	if (!manifestPath.empty())
	{
		LogD("[WorldStateMachine] Loading assets from manifest [" + manifestPath + "]...");
		Services::Engine()->Repository()->LoadAsync(manifestPath, [&]() { m_bLoaded = true; });
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
		LogD("[WorldStateMachine] ...Loaded " + World::LogName(*m_pNextState));
	}
}

void WorldStateMachine::LoadingTick(Time dt)
{
	if (m_uLoadingUI)
		m_uLoadingUI->Tick(dt);
	if (m_bLoaded)
	{
		loadTime = Time::Now() - loadTime;
		LogD("[WorldStateMachine] ...Manifest load complete in " +
			 Strings::ToString(loadTime.AsSeconds()) + "s. Loading World 0");
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
		LogD("[WorldStateMachine] Loading " + World::LogName(*m_pNextState) + "...");
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
