#pragma once
#include "SimpleTime.h"
#include "WorldObject.h"

namespace LittleEngine
{
class Entity : public WorldObject
{
private:
	Vec<class Component*> m_pComponents;

protected:
	bool m_bDestroyed = false;

public:
	Transform m_transform;
	bool m_bEnabled = true;
	bool m_bReset = true;

public:
	Entity(const String& name);
	virtual ~Entity();

	virtual void Destruct();
	virtual void SetEnabled(bool bEnabled);
	virtual void Tick(Time dt);

	template <typename T>
	T* AddComponent();

	template <typename T>
	T* GetComponent();

private:
	friend class GameManager;
	friend class Component;
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
			return t;
	}
	return nullptr;
}
} // namespace LittleEngine
