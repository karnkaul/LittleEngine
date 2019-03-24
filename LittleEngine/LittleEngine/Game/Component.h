#pragma once
#include "SimpleTime.h"
#include "WorldObject.h"

namespace LittleEngine
{
enum class TimingType
{
	FIRST = 0,
	PRE = 1,
	DEFAULT = 2,
	POST = 3,
	LAST = 4,
};

class Component : public WorldObject
{
protected:
	class Entity* m_pOwner;
	bool m_bDestroyed = false;

public:
	bool m_bEnabled = true;

public:
	Component();
	virtual ~Component();

	virtual TimingType GetComponentTiming() const;
	virtual void OnCreated();
	virtual void SetEnabled(bool bEnabled);
	virtual void Tick(Time dt);

private:
	void Destruct();
	void SetOwner(Entity& owner);

private:
	friend class Entity;
	friend class GameManager;
};
} // namespace LittleEngine
