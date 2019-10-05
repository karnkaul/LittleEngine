#include "Core/Logger.h"
#include "Inheritable.h"

namespace LittleEngine
{
Inheritable::Inheritable(bool bSilent) : m_bSilent(bSilent) {}

Inheritable::Inheritable(std::string name, std::string className, bool bSilent)
	: m_name(std::move(name)), m_typeName(std::move(className)), m_bSilent(bSilent)
{
	GenerateLogNameStr();
}

Inheritable::~Inheritable()
{
	if (!m_bSilent)
	{
		LOG_D("%s destroyed", m_logName.data());
	}
}

void Inheritable::SetName(std::string name)
{
	m_name = std::move(name);
	RegenerateLogNameStr();
}

void Inheritable::SetType(std::string typeName)
{
	m_typeName = std::move(typeName);
	RegenerateLogNameStr();
}

void Inheritable::SetNameAndType(std::string name, std::string typeName)
{
	m_name = std::move(name);
	m_typeName = std::move(typeName);
	RegenerateLogNameStr();
}

std::string_view Inheritable::Name() const
{
	return m_name;
}

std::string_view Inheritable::LogName() const
{
	return m_logName;
}

void Inheritable::RegenerateLogNameStr()
{
	GenerateLogNameStr();
}

void Inheritable::GenerateLogNameStr()
{
	m_logName.clear();
	m_logName.reserve(m_name.size() + m_typeName.size() + 3);
	m_logName += "[";
	m_logName += m_name;
	if (!m_typeName.empty())
	{
		m_logName += "/";
		m_logName += m_typeName;
	}
	m_logName += "]";
}
} // namespace LittleEngine
