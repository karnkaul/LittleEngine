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

protected:
	virtual void OnCreated() = 0;

public:
	virtual void Tick(Time dt) = 0;

public:
	virtual TimingType Timing() const;
	virtual void SetEnabled(bool bEnabled);

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
