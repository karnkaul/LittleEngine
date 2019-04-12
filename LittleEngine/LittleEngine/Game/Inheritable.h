#pragma once
#include "CoreTypes.h"

namespace LittleEngine
{
class Inheritable
{
private:
	String m_logName;
	String m_name;
	String m_className;
protected:
	// Will LOG_D destruction unless m_bSilent
	bool m_bSilent = false;

public:
	// Will LOG_D destruction unless bSilent
	Inheritable(String name, String className = "", bool bSilent = false);
	virtual ~Inheritable();

public:
	void SetName(String name, String className = "");
	const char* GetNameStr() const;
	const char* LogNameStr() const;

protected:
	void RegenerateLogNameStr();
};
} // namespace LittleEngine
