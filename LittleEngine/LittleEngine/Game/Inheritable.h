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
	Inheritable(const String& name, const String& className = "");
	virtual ~Inheritable();

public:
	void SetName(const String& name, const String& className = "");
	const char* GetNameStr() const;
	const char* LogNameStr() const;

protected:
	void RegenerateLogNameStr();
};
} // namespace LittleEngine
