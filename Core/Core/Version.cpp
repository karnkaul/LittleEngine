#include "stdafx.h"
#include "Version.h"
#include "Utils.h"

namespace Core
{
namespace
{
u32 Parse(const Vec<String>& vec, size_t idx)
{
	return (vec.size() > idx) ? static_cast<u32>(Strings::ToS32(vec[idx], 0)) : 0;
}
}

Version::Version() = default;

Version::Version(u32 major, u32 minor, u32 patch, u32 pre)
	: major(major), minor(minor), patch(patch), pre(pre)
{
}

Version::Version(const char* szSerialised)
{
	String serialised(szSerialised);
	Vec<String> tokens = Strings::Tokenise(serialised, '.', {});
	major = Parse(tokens, 0);
	minor = Parse(tokens, 1);
	patch = Parse(tokens, 2);
	pre = Parse(tokens, 3);
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
	return Strings::ToString(major) + "." + Strings::ToString(minor) + "." + Strings::ToString(patch) + "." + Strings::ToString(pre);
}

bool Version::operator==(const Version& rhs)
{
	return major == rhs.major && minor == rhs.minor && patch == rhs.patch && pre == rhs.pre;
}

bool Version::operator!=(const Version& rhs)
{
	return !(*this == rhs);
}

bool Version::operator<(const Version& rhs)
{
	return (major < rhs.major) || (major == rhs.major && minor < rhs.minor) ||
		   (major == rhs.major && minor == rhs.minor && patch < rhs.patch) ||
		   (major == rhs.major && minor == rhs.minor && patch == rhs.patch && rhs.pre < rhs.pre);
}

bool Version::operator<=(const Version& rhs)
{
	return (*this == rhs) || (*this < rhs);
}

bool Version::operator>(const Version& rhs)
{
	return (major > rhs.major) || (major == rhs.major && minor > rhs.minor) ||
		   (major == rhs.major && minor == rhs.minor && patch > rhs.patch) ||
		   (major == rhs.major && minor == rhs.minor && patch == rhs.patch && pre > rhs.pre);
}

bool Version::operator>=(const Version& rhs)
{
	return (*this == rhs) || (*this > rhs);
}
} // namespace Core