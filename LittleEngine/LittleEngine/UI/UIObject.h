#pragma once
#include "CoreTypes.h"
#include "SFMLAPI/System/SFTime.h"

namespace LittleEngine
{
class UIObject
{
protected:
	String m_name;
	String m_baseClass = "UIObj";
	bool m_bDestroyed = false;

public:
	UIObject(const String& name);
	virtual ~UIObject();

	virtual void Tick(Time dt) = 0;

	String LogName() const;

protected:
	class EngineRepository* Repository() const;
};
} // namespace LittleEngine
