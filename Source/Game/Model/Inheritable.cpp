#include "Core/Logger.h"
#include "Inheritable.h"

namespace LittleEngine
{
Inheritable::Inheritable(bool bSilent) : m_bSilent(bSilent) {}

Inheritable::Inheritable(String name, String className, bool bSilent)
	: m_name(std::move(name)), m_typeName(std::move(className)), m_bSilent(bSilent)
{
	String suffix = m_typeName.empty() ? "" : "/" + m_typeName;
	m_logName = "[" + m_name + suffix + "]";
}

Inheritable::~Inheritable()
{
	if (!m_bSilent)
	{
		LOG_D("%s destroyed", m_logName.data());
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

VString Inheritable::Name() const
{
	return m_name;
}

VString Inheritable::LogName() const
{
	return m_logName;
}

void Inheritable::RegenerateLogNameStr()
{
	String suffix = m_typeName.empty() ? "" : "/" + m_typeName;
	m_logName = "[" + m_name + suffix + "]";
}
} // namespace LittleEngine
