#include "Core/LECoreGame/LECoreUtils/Logger.h"
#include "Core/LECoreGame/LECoreUtils/Utils.h"
#include "SFMLAPI/System/Assets.h"
#include "Engine/Audio/LEAudio.h"
#include "Engine/Context/LEContext.h"
#include "Engine/Repository/ManifestLoader.h"
#include "Engine/Repository/LERepository.h"
#include "World.h"
#include "WorldClock.h"
#include "WorldStateMachine.h"
#include "Model/GameKernel.h"
#include "Model/GameManager.h"
#include "Model/UI/UIManager.h"

namespace LittleEngine
{
World::World(std::string name) : GameObject(std::move(name), "World")
{
	m_uWorldClock = std::make_unique<WorldClock>();
	m_manifestID.reserve(m_name.size() + 4);
	m_manifestID += m_name;
	m_manifestID += ".amf";
}

World::~World() = default;

bool World::LoadWorld(WorldID id)
{
	Assert(m_pWSM, "WSM is null!");
	return m_pWSM->LoadWorld(id);
}

void World::Quit()
{
	g_pGameManager->Quit();
}

Time World::WorldTime() const
{
	return m_uWorldClock->Elapsed();
}

LEInput* World::Input() const
{
	Assert(m_pWSM->m_pContext, "WSM is null!");
	return m_pWSM->m_pContext->Input();
}

void World::Init(WorldStateMachine& wsm)
{
	m_pWSM = &wsm;
	m_state = State::Inactive;
}

void World::Activate()
{
	Assert(m_pWSM, "WSM is null!");
	m_state = State::Active;
	m_uWorldClock->Restart();
	OnActivated();
	LOG_D("%s Activated", m_logName.c_str());
}

void World::Deactivate()
{
	m_tokenHandler.Clear();
	OnDeactivating();
	m_state = State::Inactive;
	LOG_D("%s Deactivated", m_logName.c_str());
}
} // namespace LittleEngine
