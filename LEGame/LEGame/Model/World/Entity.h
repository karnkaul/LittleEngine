#pragma once
#include "Core/SimpleTime.h"
#include "GameObject.h"

namespace LittleEngine
{
class Entity : public GameObject
{
protected:
	using Super = Entity;

private:
	Vec<class AComponent*> m_pComponents;
#if DEBUGGING
public:
	static bool s_bShowOrientation;
	static Vector2 s_orientationWidthHeight;
	static Array<Colour, 2> s_xyColours;
protected:
	class SFRect* m_pO_x = nullptr;
	SFRect* m_pO_y = nullptr;
#endif

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
	Assert(g_pGameManager, "Game Manager is null!");
	return g_pGameManager->NewComponent<T>(*this);
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
