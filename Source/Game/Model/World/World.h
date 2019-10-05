#pragma once
#include "Core/CoreTypes.h"
#include "Engine/Input/LEInput.h"
#include "GameObject.h"

namespace LittleEngine
{
using WorldID = s32;

class World : public GameObject
{
protected:
	using Super = World;

	enum class State
	{
		Invalid = 0,
		Loading,
		Active,
		Inactive
	};

private:
	std::string m_manifestID;
	UPtr<class WorldClock> m_uWorldClock;
	State m_state;
	class WorldStateMachine* m_pWSM = nullptr;

protected:
	WorldID m_id;
	bool m_bTickWhenPaused = false;

public:
	World(std::string name);
	~World() override;

	bool LoadWorld(WorldID id);
	void Quit();

	Time WorldTime() const;
	class LEInput* Input() const;

protected:
	virtual void OnActivated() = 0;
	virtual void Tick(Time dt) = 0;
	virtual void OnDeactivating() = 0;

private:
	void Init(WorldStateMachine& wsm);
	void Activate();
	void Deactivate();

private:
	friend class WorldStateMachine;
};
} // namespace LittleEngine
