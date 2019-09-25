#pragma once
#include "Core/CoreTypes.h"

namespace LittleEngine
{
class Inheritable
{
protected:
	String m_logName;
	String m_name;
	String m_typeName;

protected:
	// Will LOG_D destruction unless m_bSilent
	bool m_bSilent = false;

public:
	Inheritable(bool bSilent = false);
	// Will LOG_D destruction unless bSilent
	Inheritable(String name, String className = "", bool bSilent = false);
	virtual ~Inheritable();

public:
	void SetName(String name);
	void SetType(String typeName);
	void SetNameAndType(String name, String className);
	VString Name() const;
	VString LogName() const;

protected:
	virtual void RegenerateLogNameStr();
};
} // namespace LittleEngine
