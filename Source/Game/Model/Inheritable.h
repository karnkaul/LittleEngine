#pragma once
#include "Core/CoreTypes.h"

namespace LittleEngine
{
class Inheritable
{
protected:
	std::string m_logName;
	std::string m_name;
	std::string m_typeName;

protected:
	// Will LOG_D destruction unless m_bSilent
	bool m_bSilent = false;

public:
	Inheritable(bool bSilent = false);
	// Will LOG_D destruction unless bSilent
	Inheritable(std::string name, std::string className = "", bool bSilent = false);
	virtual ~Inheritable();

public:
	void SetName(std::string name);
	void SetType(std::string typeName);
	void SetNameAndType(std::string name, std::string typeName);
	std::string_view Name() const;
	std::string_view LogName() const;

protected:
	virtual void RegenerateLogNameStr();

private:
	void GenerateLogNameStr();
};
} // namespace LittleEngine
