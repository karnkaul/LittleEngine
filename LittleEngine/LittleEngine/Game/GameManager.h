#pragma once
#include "Entity.h"
#include "Component.h"
#include "LittleEngine/UI/UIManager.h"
#include "LittleEngine/Physics/CollisionManager.h"
#include "LittleEngine/Services/IService.h"

namespace LittleEngine
{
class GameManager final : public IService
{
private:
	static constexpr size_t COMPONENT_LINES = static_cast<size_t>(TimingType::LAST) + 1;

private:
	String m_logName;
	Vector<UPtr<Entity>> m_uEntities;
	Array<Vector<UPtr<Component>>, COMPONENT_LINES> m_uComponents;
	UPtr<UIManager> m_uUIManager;
	UPtr<CollisionManager> m_uCollisionManager;

public:
	GameManager();
	~GameManager();

	UIManager* UI() const;
	EngineInput* Input() const;
	EngineAudio* Audio() const;
	EngineRepository* Repository() const;
	WorldStateMachine* Worlds() const;
	CollisionManager* Physics() const;

public:
	template <typename T>
	T* NewEntity(const String& name, const Vector2& position = Vector2::Zero, const Fixed& orientation = Fixed::Zero);
	template <typename T>
	T* NewComponent(Entity& owner);

	const char* LogNameStr() const;

private:
	void Tick(Time dt);

	friend class World;
};

template <typename T>
T* GameManager::NewEntity(const String& name, const Vector2& position, const Fixed& orientation)
{
	static_assert(IsDerived(Entity, T), "T must derive from Entity");
	UPtr<T> uT = MakeUnique<T>(name);
	T* pT = uT.get();
	m_uEntities.emplace_back(std::move(uT));
	pT->m_transform.localPosition = position;
	pT->m_transform.localOrientation = orientation;
	LOG_D("%s spawned", pT->LogNameStr());
	return pT;
}

template <typename T>
T* GameManager::NewComponent(Entity& owner)
{
	static_assert(IsDerived(Component, T), "T must derive from Entity");
	UPtr<T> uT = MakeUnique<T>();
	size_t idx = static_cast<size_t>(uT->GetComponentTiming());
	Assert(m_uComponents.size() > idx, "Invalid Component Timing index!");
	auto& componentVec = m_uComponents.at(idx);
	uT->SetOwner(owner);
	T* pT = uT.get();
	componentVec.emplace_back(std::move(uT));
	LOG_D("%s constructed and attached to %s", pT->LogNameStr(), owner.LogNameStr());
	return pT;
}
} // namespace LittleEngine
