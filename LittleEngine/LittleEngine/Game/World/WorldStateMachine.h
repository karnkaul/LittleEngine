#pragma once
#include "World.h"

namespace LittleEngine
{
class WorldStateMachine final
{
public:
	static bool s_bReady;

private:
	Vec<UPtr<World>> m_uCreatedStates;
	UPtr<class LoadingUI> m_uLoadingUI;
	World* m_pActiveState = nullptr;
	World* m_pNextState = nullptr;
	class ManifestLoader* m_pAssetLoader = nullptr;
	bool m_bToActivateState = false;
	bool m_bLoading = false;
	bool m_bLoaded = false;
	String m_manifestPath;
	String m_archivePath;
#if DEBUGGING
public:
	static bool s_bTEST_infiniteLoad;
#endif

public:
	WorldStateMachine();
	~WorldStateMachine();

	template <typename T>
	WorldID CreateWorld();
	World* GetActiveState() const;
	WorldID GetActiveStateID() const;
	Vec<WorldID> GetAllStateIDs() const;
	bool LoadState(WorldID id);

private:
	void Start(String manifestPath = "", String archivePath = "");
	void Tick(Time dt);
	void PostBufferSwap();

	void LoadingTick(Time dt);
	void GameTick(Time dt);

	friend class EngineService;
};

template <typename T>
WorldID WorldStateMachine::CreateWorld()
{
	WorldID id = -1;
	static_assert(IsDerived<World, T>(), "T must derive from World!");
	UPtr<T> uState = MakeUnique<T>();
	if (uState)
	{
		id = m_uCreatedStates.size();
		uState->m_id = id;
		m_uCreatedStates.push_back(std::move(uState));
	}
	return id;
}
} // namespace LittleEngine
