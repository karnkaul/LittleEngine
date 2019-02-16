#include "stdafx.h"
#include "Inheritable.h"

namespace LittleEngine
{
Inheritable::Inheritable(const String& name, const String& className)
	: m_name(name), m_className(className)
{
	RegenerateLogNameStr();
}

Inheritable::~Inheritable() = default;

void Inheritable::SetName(const String& name, const String& className)
{
	if (!name.empty())
		m_name = name;
	if (!className.empty())
		m_className = className;
	RegenerateLogNameStr();
}

const char* Inheritable::GetNameStr() const
{
	return m_name.c_str();
}

const char* Inheritable::LogNameStr() const
{
	return m_logName.c_str();
}

void Inheritable::RegenerateLogNameStr()
{
	String suffix = m_className.empty() ? "" : " (" + m_className + ")";
	m_logName = "[" + m_name + suffix + "]";
}
} // namespace LittleEngine
