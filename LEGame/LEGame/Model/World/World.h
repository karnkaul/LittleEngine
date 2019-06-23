#pragma once
#include "Core/CoreTypes.h"
#include "LittleEngine/Input/LEInput.h"
#include "GameObject.h"

namespace LittleEngine
{
using WorldID = s32;

class World : public GameObject
{
protected:
	using Super = World;

private:
	UPtr<class WorldClock> m_uWorldClock;
	class WorldStateMachine* m_pWSM = nullptr;
	
protected:
	WorldID m_id;
	UPtr<class GameManager> m_uGame;

public:
	World(String name);
	~World() override;

	bool LoadWorld(WorldID id);
	void Quit();

	Time GetWorldTime() const;
	class LEInput* Input() const;

protected:
	virtual void PreTick(Time dt) = 0;
	virtual void PostTick(Time dt) = 0;
	virtual void OnActivated() = 0;
	virtual void OnDeactivating() = 0;

protected:
	virtual void OnClearing();

private:
	void Tick(Time dt);
	void Activate();
	void Deactivate();
	void Clear();

private:
	friend class WorldStateMachine;
};
} // namespace LittleEngine
