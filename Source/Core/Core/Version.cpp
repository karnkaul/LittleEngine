#include "Version.h"
#include "Utils.h"
#include "BuildVersion.h"

#if __linux__
#undef major
#undef minor
#undef patch
#endif

namespace Core
{
namespace
{
u32 Parse(const Vec<String>& vec, size_t idx)
{
	return (vec.size() > idx) ? static_cast<u32>(Strings::ToS32(vec[idx], 0)) : 0;
}
} // namespace

const Version Version::BUILD_VERSION = Version(BUILD_VERSION_MAJOR, BUILD_VERSION_MINOR, BUILD_VERSION_PATCH, BUILD_VERSION_PRE);
const VString Version::szBUILD_VERSION_WITH_COMMIT = BUILD_VERSION_FULL;

Version::Version() = default;

Version::Version(u32 major, u32 minor, u32 patch, u32 pre) : major(major), minor(minor), patch(patch), pre(pre) {}

Version::Version(VString serialised)
{
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

bool Version::Upgrade(const Version& rhs)
{
	if (*this < rhs)
	{
		*this = rhs;
		return true;
	}
	return false;
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
	return (major < rhs.major) || (major == rhs.major && minor < rhs.minor)
		   || (major == rhs.major && minor == rhs.minor && patch < rhs.patch)
		   || (major == rhs.major && minor == rhs.minor && patch == rhs.patch && pre < rhs.pre);
}

bool Version::operator<=(const Version& rhs)
{
	return (*this == rhs) || (*this < rhs);
}

bool Version::operator>(const Version& rhs)
{
	return (major > rhs.major) || (major == rhs.major && minor > rhs.minor)
		   || (major == rhs.major && minor == rhs.minor && patch > rhs.patch)
		   || (major == rhs.major && minor == rhs.minor && patch == rhs.patch && pre > rhs.pre);
}

bool Version::operator>=(const Version& rhs)
{
	return (*this == rhs) || (*this > rhs);
}
} // namespace Core
