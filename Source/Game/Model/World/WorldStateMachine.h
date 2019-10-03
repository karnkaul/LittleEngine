#pragma once
#include "Core/Delegate.h"
#include "World.h"

namespace LittleEngine
{
class WorldStateMachine final
{
private:
	enum class State
	{
		Idle = 0,
		Loading,
		Running,
		Quitting
	};

	enum class Transition
	{
		None = 0,
		UnloadLoad,
		UnloadRun,
		LoadRun,
		Run
	};

private:
	static Vec<UPtr<class World>> s_createdWorlds;

private:
	String m_manifestPath;
	Task m_onLoaded;
	Time m_loadTime;
	Token m_onSubmitToken;
	Token m_inputToken;
	State m_state;
	Transition m_transition;
	UPtr<class ILoadingHUD> m_uLoadHUD;
	SPtr<class ManifestLoader> m_sLoader;
	class LEContext* m_pContext;
	class SFText* m_pLoadingTitle = nullptr;
	SFText* m_pLoadingSubtitle = nullptr;
	class Quad* m_pSpinner = nullptr;
	World* m_pActiveWorld = nullptr;
	World* m_pNextWorld = nullptr;
	
public:
#ifdef DEBUGGING
	static bool s_bRunning;
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

	void Start(String coreManifestID = "", String gameStyleID = "", Task onManifestLoaded = nullptr);
	State Tick(Time dt);
	bool LoadWorld(WorldID id);
	void SetLoadingHUD(UPtr<class ILoadingHUD> uLoadingHUD);

	World* ActiveWorld() const;
	WorldID ActiveWorldID() const;
	Vec<WorldID> AllWorldIDs() const;

private:
	void ChangeState();

	void UnloadActiveWorld();
	void Quit();

	friend class GameManager;
	friend class World;
};

template <typename T>
WorldID WorldStateMachine::CreateWorld()
{
	WorldID id = -1;
	static_assert(IsDerived<World, T>(), "T must derive from World!");
	UPtr<T> uWorld = MakeUnique<T>();
	if (uWorld)
	{
		id = ToS32(s_createdWorlds.size());
		uWorld->m_id = id;
		s_createdWorlds.push_back(std::move(uWorld));
	}
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
