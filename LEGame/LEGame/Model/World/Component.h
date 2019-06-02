#pragma once
#include "Core/SimpleTime.h"
#include "ComponentTimingType.h"
#include "GameObject.h"

namespace LittleEngine
{
class AComponent : public GameObject
{
protected:
	using Super = AComponent;

protected:
	class Entity* m_pOwner = nullptr;
	bool m_bDestroyed = false;

public:
	bool m_bEnabled = true;

public:
	AComponent();
	~AComponent() override;
	
	virtual void OnCreated() = 0;
	
	virtual TimingType GetComponentTiming() const;
	virtual void SetEnabled(bool bEnabled);
	virtual void Tick(Time dt);

protected:
	void RegenerateLogNameStr() override;

private:
	void Destruct();
	void OnCreate(Entity& owner);

private:
	friend class Entity;
	friend class GameManager;
};
} // namespace LittleEngine
