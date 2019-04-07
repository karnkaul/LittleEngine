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

public:
	Inheritable(String name, String className = "");
	virtual ~Inheritable();

public:
	void SetName(String name, String className = "");
	const char* GetNameStr() const;
	const char* LogNameStr() const;

protected:
	void RegenerateLogNameStr();
};
} // namespace LittleEngine
