#include "stdafx.h"
#include "Inheritable.h"

namespace LittleEngine
{
Inheritable::Inheritable(String name, String className)
	: m_name(std::move(name)), m_className(std::move(className))
{
	RegenerateLogNameStr();
}

Inheritable::~Inheritable() = default;

void Inheritable::SetName(String name, String className)
{
	if (!name.empty())
	{
		m_name = std::move(name);
	}
	if (!className.empty())
	{
		m_className = std::move(className);
	}
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
