#include "stdafx.h"
#include "World.h"
#include "Utils.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/UI/UIManager.h"
#include "LittleEngine/Game/GameManager.h"

namespace LittleEngine
{
World::World(const String& name) : m_name(name)
{
}
World::~World() = default;

void World::PlaySFX(SoundAsset* pSound, const Fixed& volume, const Fixed& direction, bool bLoop)
{
	if (pSound)
		Services::Engine()->Audio()->PlaySFX(*pSound, volume, direction, bLoop);
}

void World::PlayMusic(const String& path, const Fixed& volume, Time fadeTime, bool bLoop)
{
	Services::Engine()->Audio()->PlayMusic(path, volume, fadeTime, bLoop);
}

void World::BindInput(EngineInput::Delegate Callback)
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
	return m_worldClock.GetElapsed();
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
	LogD(LogName(*this) + " Activated.");
	m_worldClock.Restart();
	m_uGame = MakeUnique<GameManager>(*this);
	OnActivated();
}

void World::Deactivate()
{
	OnDeactivating();
	m_uGame = nullptr;
	Clear();
	LogD(LogName(*this) + " Deactivated");
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

String World::LogName(const World& world)
{
	return "[" + world.m_name + " (World " + Strings::ToString(world.m_id) + ")]";
}
} // namespace LittleEngine
