#include "stdafx.h"
#include "World.h"
#include "Core/Utils.h"
#include "SFMLAPI/System/SFAssets.h"
#include "SFMLAPI/System/SFGameClock.h"
#include "LittleEngine/Audio/LEAudio.h"
#include "LittleEngine/Context/LEContext.h"
#include "LittleEngine/Repository/LERepository.h"
#include "WorldStateMachine.h"
#include "LEGame/Model/GameManager.h"

namespace LittleEngine
{
World::World(String name) : GameObject(std::move(name), "World")
{
	m_uWorldClock = MakeUnique<GameClock>();
}

World::~World() = default;

bool World::LoadWorld(WorldID id)
{
	Assert(m_pWSM, "WSM is null!");
	return m_pWSM->LoadState(id);
}

void World::Quit()
{
	m_uGame->Quit();
}

Time World::GetWorldTime() const
{
	return m_uWorldClock->GetElapsed();
}

LEInput* World::Input() const
{
	Assert(m_pWSM, "WSM is null!");
	return m_pWSM->m_pContext->Input();
}

void World::OnClearing()
{
}

void World::Activate()
{
	Assert(m_pWSM, "WSM is null!");
	LOG_D("%s Activated.", LogNameStr());
	m_uWorldClock->Restart();
	m_uGame = MakeUnique<GameManager>(*m_pWSM);
	OnActivated();
}

void World::Deactivate()
{
	OnDeactivating();
	m_uGame = nullptr;
	Clear();
	LOG_D("%s Deactivated", LogNameStr());
}

void World::Tick(Time dt)
{
	m_uGame->Tick(dt);
}

void World::Clear()
{
	OnClearing();
	m_uGame = nullptr;
	m_tokenHandler.Clear();
}

void World::OnActivated()
{
}
void World::OnDeactivating()
{
}
} // namespace LittleEngine
