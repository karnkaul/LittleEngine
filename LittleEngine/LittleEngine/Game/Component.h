#pragma once
#include "SimpleTime.h"
#include "ComponentTimingType.h"
#include "WorldObject.h"

namespace LittleEngine
{
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
