#pragma once
#include "SimpleTime.h"
#include "WorldObject.h"

namespace LittleEngine
{
class Entity : public WorldObject
{
private:
	Vec<class AComponent*> m_pComponents;

protected:
	bool m_bDestroyed = false;

public:
	Transform m_transform;
	bool m_bEnabled = true;
	bool m_bResetRenderState = true;

public:
	Entity();
	~Entity() override;

	void OnCreate(String name);
	virtual void Destruct();
	virtual void SetEnabled(bool bEnabled);
	virtual void Tick(Time dt);

	template <typename T>
	T* AddComponent();

	template <typename T>
	T* GetComponent();

protected:
	virtual void OnCreated();

private:
	friend class GameManager;
	friend class AComponent;
};

template <typename T>
T* Entity::AddComponent()
{
	return Services::Game()->NewComponent<T>(*this);
}

template <typename T>
T* Entity::GetComponent()
{
	for (auto pComponent : m_pComponents)
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
