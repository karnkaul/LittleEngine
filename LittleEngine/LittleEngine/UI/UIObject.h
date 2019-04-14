#pragma once
#include "CoreTypes.h"
#include "SimpleTime.h"
#include "LittleEngine/Game/Inheritable.h"

namespace LittleEngine
{
class UIObject : public Inheritable
{
protected:
	bool m_bDestroyed = false;

public:
	UIObject(bool bSilent = false);
	UIObject(String name, bool bSilent = false);
	~UIObject() override;

	virtual void Tick(Time dt) = 0;

protected:
	class EngineRepository* Repository() const;
};
} // namespace LittleEngine
