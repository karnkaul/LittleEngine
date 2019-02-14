#pragma once
#include "CoreTypes.h"

namespace LittleEngine
{
class Inheritable
{
public:
	String m_name;
	String m_baseClass;

public:
	Inheritable(const String& name, const String& baseClass = "");
	virtual ~Inheritable();

	String LogName() const;
};
} // namespace LittleEngine
