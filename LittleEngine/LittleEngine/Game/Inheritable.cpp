#include "stdafx.h"
#include "Inheritable.h"

namespace LittleEngine
{
Inheritable::Inheritable(const String& name, const String& baseClass)
	: m_name(name), m_baseClass(baseClass)
{
}

Inheritable::~Inheritable() = default;

String Inheritable::LogName() const
{
	String suffix = m_baseClass.empty() ? "" : " (" + m_baseClass + ")";
	return "[" + m_name + suffix + "]";
}
} // namespace LittleEngine
