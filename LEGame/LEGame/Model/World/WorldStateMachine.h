#pragma once
#include "World.h"

namespace LittleEngine
{
class WorldStateMachine final
{
public:
	static bool s_bReady;
private:
	static bool s_bHasState;
	static Vec<UPtr<World>> s_uCreatedStates;
private:
	UPtr<class WSMLoadingUI> m_uLoadingUI;
	UPtr<class WSMErrorUI> m_uErrorUI;
	World* m_pActiveState = nullptr;
	World* m_pNextState = nullptr;
	class ManifestLoader* m_pAssetLoader = nullptr;
	class LEContext* m_pContext;
	bool m_bToActivateState = false;
	bool m_bLoading = false;
	bool m_bLoaded = false;
	String m_manifestPath;
#if DEBUGGING
public:
	static bool s_bTEST_infiniteLoad;
#endif

public:
	template <typename T>
	static WorldID CreateWorld();
	template <typename T>
	static void CreateWorlds();
	template <typename T, typename U, typename... V>
	static void CreateWorlds();

public:
	WorldStateMachine(LEContext& context);
	~WorldStateMachine();

	World* GetActiveState() const;
	WorldID GetActiveStateID() const;
	Vec<WorldID> GetAllStateIDs() const;
	bool LoadState(WorldID id);

	void Start(String manifestPath = "", String gameStyleID = "");
	// Returns true if active World state changed
	bool Tick(Time dt);

	void LoadingTick(Time dt);
	// Returns true if active World state changed
	bool GameTick(Time dt);

private:
	void Quit();

	friend class GameManager;
	friend class World;
};

template <typename T>
WorldID WorldStateMachine::CreateWorld()
{
	WorldID id = -1;
	static_assert(IsDerived<World, T>(), "T must derive from World!");
	UPtr<T> uState = MakeUnique<T>();
	if (uState)
	{
		id = s_uCreatedStates.size();
		uState->m_id = id;
		s_uCreatedStates.push_back(std::move(uState));
	}
	s_bHasState = true;
	return id;
}

template <typename T>
void WorldStateMachine::CreateWorlds()
{
	CreateWorld<T>();
}

template <typename T, typename U, typename... V>
void WorldStateMachine::CreateWorlds()
{
	CreateWorld<T>();
	CreateWorlds<U, V...>();
}
} // namespace LittleEngine
