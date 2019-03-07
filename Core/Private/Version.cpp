#include "stdafx.h"
#include "Version.h"
#include "Utils.h"

namespace Core
{
Version::Version() : major(0), minor(0), patch(0)
{
}

Version::Version(u32 major, u32 minor, u32 patch) : major(major), minor(minor), patch(patch)
{
}

Version::Version(const char* szZerialised) : major(0), minor(0), patch(0)
{
	String serialised(szZerialised);
	Vec<String> tokens = Strings::Tokenise(serialised, '.', {});
	if (tokens.size() > 0)
		major = static_cast<u32>(Strings::ToS32(tokens[0], 0));
	if (tokens.size() > 1)
		minor = static_cast<u32>(Strings::ToS32(tokens[1], 0));
	if (tokens.size() > 2)
		patch = static_cast<u32>(Strings::ToS32(tokens[2], 0));
}

u32 Version::Major() const
{
	return major;
}

u32 Version::Minor() const
{
	return minor;
}

u32 Version::Patch() const
{
	return patch;
}

String Version::ToString() const
{
	return Strings::ToString(major) + "." + Strings::ToString(minor) + "." + Strings::ToString(patch);
}

bool Version::operator==(const Version& rhs)
{
	return major == rhs.major && minor == rhs.minor && patch == rhs.patch;
}

bool Version::operator!=(const Version& rhs)
{
	return !(*this == rhs);
}

bool Version::operator<(const Version& rhs)
{
	return (major < rhs.major) || (major == rhs.major && minor < rhs.minor) ||
		   (major == rhs.major && minor == rhs.minor && patch < rhs.patch);
}

bool Version::operator<=(const Version& rhs)
{
	return (*this == rhs) || (*this < rhs);
}

bool Version::operator>(const Version& rhs)
{
	return (major > rhs.major) || (major == rhs.major && minor > rhs.minor) ||
		   (major == rhs.major && minor == rhs.minor && patch > rhs.patch);
}

bool Version::operator>=(const Version& rhs)
{
	return (*this == rhs) || (*this > rhs);
}
} // namespace Core