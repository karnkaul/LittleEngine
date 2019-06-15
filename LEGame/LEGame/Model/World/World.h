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
	UPtr<class GameClock> m_uWorldClock;
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
