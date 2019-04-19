#pragma once
#include "CoreTypes.h"
#include "TokenHandler.hpp"
#include "LittleEngine/Game/Inheritable.h"
#include "LittleEngine/Input/EngineInput.h"

namespace LittleEngine
{
using WorldID = s32;

class World : public Inheritable
{
private:
	UPtr<class GameClock> m_uWorldClock;
	
protected:
	Core::TokenHandler<EngineInput::Token> m_tokenHandler;
	WorldID m_id;
	UPtr<class GameManager> m_uGame;

public:
	World(String name);
	~World() override;

	void PlaySFX(class SoundAsset* pSound, Fixed volume, Fixed direction, bool bLoop);
	void PlayMusic(String path, Fixed volume, Time fadeTime, bool bLoop);
	void BindInput(EngineInput::Delegate callback);
	bool LoadWorld(WorldID id);
	void Quit();

	Time GetWorldTime() const;
	class EngineRepository* Repository() const;
	class EngineAudio* Audio() const;
	class EngineInput* Input() const;
	class GameManager* Game() const;

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
