#include "stdafx.h"
#include "World.h"
#include "Utils.h"
#include "SFMLAPI/System/SFAssets.h"
#include "SFMLAPI/System/SFGameClock.h"
#include "WorldStateMachine.h"
#include "LittleEngine/Audio/EngineAudio.h"
#include "LittleEngine/Engine/EngineService.h"
#include "LittleEngine/Game/Camera.h"
#include "LittleEngine/Game/GameManager.h"
#include "LittleEngine/Repository/EngineRepository.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/UI/UIManager.h"

namespace LittleEngine
{
World::World(const String& name) : Inheritable(name, "World")
{
	m_uWorldClock = MakeUnique<GameClock>();
}

World::~World() = default;

void World::PlaySFX(SoundAsset* pSound, Fixed volume, Fixed direction, bool bLoop)
{
	if (pSound)
	{
		Services::Engine()->Audio()->PlaySFX(*pSound, volume, direction, bLoop);
	}
}

void World::PlayMusic(const String& path, Fixed volume, Time fadeTime, bool bLoop)
{
	Services::Engine()->Audio()->PlayMusic(path, volume, fadeTime, bLoop);
}

void World::BindInput(const EngineInput::Delegate& Callback)
{
	m_tokenHandler.AddToken(Services::Engine()->Input()->Register(Callback));
}

bool World::LoadWorld(WorldID id)
{
	return Services::Engine()->Worlds()->LoadState(id);
}

void World::Quit()
{
	Services::Engine()->Terminate();
}

Time World::GetWorldTime() const
{
	return m_uWorldClock->GetElapsed();
}

EngineRepository* World::Repository() const
{
	return Services::Engine()->Repository();
}

EngineAudio* World::Audio() const
{
	return Services::Engine()->Audio();
}

EngineInput* World::Input() const
{
	return Services::Engine()->Input();
}

GameManager* World::Game() const
{
	return m_uGame.get();
}

void World::OnClearing()
{
}

void World::Activate()
{
	LOG_D("%s Activated.", LogNameStr());
	m_uWorldClock->Restart();
	m_uGame = MakeUnique<GameManager>();
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
