#pragma once
#include "Core/CoreTypes.h"
#include "Core/Logger.h"
#include "Model/World/ComponentTimingType.h"

namespace LittleEngine
{
using WorldID = s32;

// Note: pointer will reset with every World activation
extern class GameManager* g_pGameManager;

class GameManager final
{
private:
	static constexpr size_t COMPONENT_LINES = ToIdx(TimingType::Last) + 1;

private:
	String m_logName;
	Vec<Task> m_initCallbacks;
	Vec<UPtr<class Entity>> m_uEntities;
	Array<Vec<UPtr<class AComponent>>, COMPONENT_LINES> m_uComponents;
	UPtr<class LEContext> m_uContext;
	UPtr<class WorldStateMachine> m_uWSM;
	UPtr<class UIManager> m_uUIManager;
	UPtr<class LEPhysics> m_uCollisionManager;
	UPtr<class Camera> m_uWorldCamera;
	bool m_bQuitting = false;
	bool m_bPaused = false;

public:
	GameManager();
	~GameManager();

	UIManager* UI() const;
	class LEInput* Input() const;
	class LERenderer* Renderer() const;
	class LEContext* Context() const;
	Camera* WorldCamera() const;
	LEPhysics* Physics() const;

	bool LoadWorld(WorldID id);
	WorldID ActiveWorldID() const;
	Vec<WorldID> AllWorldIDs() const;

	void Start(String coreManifestID = "", String gameStyleID = "", Task onManifestLoaded = nullptr);
	void SetPaused(bool bPaused);
	void Quit();
	void SetWorldCamera(UPtr<Camera> uCamera);

	bool IsPaused() const;
	bool IsPlayerControllable() const;

	//Vector2 WorldSize() const;
	//Vector2 UISize() const;
	//Vector2 WorldProjection(Vector2 nPos) const;
	//Vector2 UIProjection(Vector2 nPos) const;

public:
	template <typename T>
	T* NewEntity(String name = "Untitled", Vector2 position = Vector2::Zero, Vector2 orientation = Vector2::Right);
	template <typename T>
	T* NewComponent(Entity& owner);

	const char* LogNameStr() const;

public:
	void CreateContext(const class GameConfig& config);
	void ModifyTickRate(Time newTickRate);
	void Reset();
	void Tick(Time dt, bool& bYieldIntegration);
};

template <typename T>
T* GameManager::NewEntity(String name, Vector2 position, Vector2 orientation)
{
	static_assert(IsDerived<Entity, T>(), "T must derive from Entity");
	UPtr<T> uT = MakeUnique<T>();
	T* pT = uT.get();
	m_uEntities.emplace_back(std::move(uT));
	pT->OnCreate(std::move(name));
	pT->m_transform.SetPosition(position);
	pT->m_transform.SetOrientation(orientation);
	LOG_D("%s spawned", pT->LogNameStr());
	return pT;
}

template <typename T>
T* GameManager::NewComponent(Entity& owner)
{
	static_assert(IsDerived<AComponent, T>(), "T must derive from AComponent");
	UPtr<T> uT = MakeUnique<T>();
	size_t idx = ToIdx(uT->Timing());
	Assert(m_uComponents.size() > idx, "Invalid Component Timing index!");
	auto& componentVec = m_uComponents.at(idx);
	uT->OnCreate(owner);
	T* pT = uT.get();
	componentVec.emplace_back(std::move(uT));
	LOG_D("%s spawned", pT->LogNameStr());
	return pT;
}
} // namespace LittleEngine
