#include "stdafx.h"
#include "Core/Logger.h"
#include "Inheritable.h"

namespace LittleEngine
{
Inheritable::Inheritable(bool bSilent) : m_bSilent(bSilent)
{
}

Inheritable::Inheritable(String name, String className, bool bSilent)
	: m_name(std::move(name)), m_typeName(std::move(className)), m_bSilent(bSilent)
{
	RegenerateLogNameStr();
}

Inheritable::~Inheritable()
{
	if (!m_bSilent)
	{
		LOG_D("%s destroyed", LogNameStr());
	}
}

void Inheritable::SetName(String name)
{
	m_name = std::move(name);
	RegenerateLogNameStr();
}

void Inheritable::SetType(String typeName)
{
	m_typeName = std::move(typeName);
	RegenerateLogNameStr();
}

void Inheritable::SetNameAndType(String name, String className)
{
	m_name = std::move(name);
	m_typeName = std::move(className);
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
	String suffix = m_typeName.empty() ? "" : "/" + m_typeName;
	m_logName = "[" + m_name + suffix + "]";
}
} // namespace LittleEngine
