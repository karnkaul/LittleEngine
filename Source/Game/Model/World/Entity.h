#pragma once
#include "Core/Game/LECoreUtils/SimpleTime.h"
#include "GameObject.h"
#include "../GameManager.h"

namespace LittleEngine
{
class Entity : public GameObject
{
public:
	enum class State
	{
		Invalid = 0,
		Spawned,
		Despawned,
		Destroyed
	};

protected:
	using Super = Entity;

private:
	std::vector<class AComponent*> m_components;
	State m_state;
	class EntityPool* m_pPool = nullptr;

#if defined(DEBUGGING)
public:
	static bool s_bShowOrientation;
	static Vector2 s_orientationWidthHeight;
	static std::array<Colour, 2> s_xyColours;

protected:
	class SFRect* m_pO_x = nullptr;
	SFRect* m_pO_y = nullptr;
#endif

public:
	Transform m_transform;
	bool m_bEnabled = true;
	bool m_bResetRenderState = true;

public:
	Entity();
	~Entity() override;

public:
	virtual void SetEnabled(bool bEnabled);
	virtual void Tick(Time dt);
	virtual void Step(Time fdt);

public:
	void OnCreate(std::string name);
	void Destruct();
	void Respawn();
	void Despawn();

	template <typename T>
	T* AddComponent();

	template <typename T>
	T* GetComponent();

	State GetState() const;
	bool IsDestroyed() const;

	Matrix3 WorldMatrix() const;

protected:
	virtual void OnCreated();
	virtual void OnRespawn();
	virtual void OnDespawn();

private:
	friend class GameManager;
	friend class AComponent;
	friend class EntityPool;
};

template <typename T>
T* Entity::AddComponent()
{
	Assert(g_pGameManager, "Game Manager is null!");
	static_assert(IsDerived<AComponent, T>(), "T must derive from AComponent");
	T* pComponent = g_pGameManager->NewComponent<T>(*this);
	if (pComponent)
	{
		pComponent->SetEnabled(m_bEnabled);
		m_components.push_back(pComponent);
	}
	return pComponent;
}

template <typename T>
T* Entity::GetComponent()
{
	for (auto pComponent : m_components)
	{
		T* t = dynamic_cast<T*>(pComponent);
		if (t)
		{
			return t;
		}
	}
	return nullptr;
}
} // namespace LittleEngine
