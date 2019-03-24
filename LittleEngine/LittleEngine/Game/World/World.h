#pragma once
#include "CoreTypes.h"
#include "TokenHandler.hpp"
#include "SimpleTime.h"
#include "SFMLAPI/System/SFGameClock.h"
#include "LittleEngine/Game/Inheritable.h"
#include "LittleEngine/Input/EngineInput.h"
#include "LittleEngine/Repository/EngineRepository.h"
#include "LittleEngine/Audio/EngineAudio.h"

namespace LittleEngine
{
using WorldID = s32;

class World : public Inheritable
{
private:
	GameClock m_worldClock;
	
protected:
	Core::TokenHandler<EngineInput::Token> m_tokenHandler;
	WorldID m_id;
	UPtr<class GameManager> m_uGame;

public:
	World(const String& name);
	virtual ~World();

	void PlaySFX(SoundAsset* pSound, const Fixed& volume, const Fixed& direction, bool bLoop);
	void PlayMusic(const String& path, const Fixed& volume, Time fadeTime, bool bLoop);
	void BindInput(EngineInput::Delegate Callback);
	bool LoadWorld(WorldID id);
	void Quit();

	Time GetWorldTime() const;
	EngineRepository* Repository() const;
	EngineAudio* Audio() const;
	EngineInput* Input() const;
	GameManager* Game() const;

protected:
	virtual void Tick(Time dt);
	virtual void OnClearing();

private:
	void Activate();
	void Deactivate();
	void Clear();

	virtual void OnActivated();
	virtual void OnDeactivating();

private:
	friend class WorldStateMachine;
};
} // namespace LittleEngine
