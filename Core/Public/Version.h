#pragma once
#include "StdTypes.h"

namespace Core
{
struct Version
{
private:
	u32 major;
	u32 minor;
	u32 patch;

public:
	Version();
	Version(u32 major, u32 minor, u32 patch);
	Version(const char* szSerialised);

public:
	u32 Major() const;
	u32 Minor() const;
	u32 Patch() const;
	String ToString() const;

	bool operator==(const Version& rhs);
	bool operator!=(const Version& rhs);
	bool operator<(const Version& rhs);
	bool operator<=(const Version& rhs);
	bool operator>(const Version& rhs);
	bool operator>=(const Version& rhs);
};
} // namespace Core