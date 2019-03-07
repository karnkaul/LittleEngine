#pragma once
#include "CoreTypes.h"
#include "LittleEngine/Game/Inheritable.h"
#include "SFMLAPI/System/SFTime.h"

namespace LittleEngine
{
class UIObject : public Inheritable
{
protected:
	bool m_bDestroyed = false;

public:
	UIObject(const String& name);
	virtual ~UIObject();

	virtual void Tick(Time dt) = 0;

protected:
	class EngineRepository* Repository() const;
};
} // namespace LittleEngine
