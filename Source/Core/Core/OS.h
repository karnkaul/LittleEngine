#pragma once
#include <optional>
#include "StdTypes.h"
#include "Vector2.h"

#if _MSC_VER
#define CXX_MS_CRT
#elif __APPLE__ && __MACH__
#define CXX_LIBSTDC
#elif __linux__ || __MINGW32__
#define CXX_LIBC
#else
#define CXX_UNDEFINED
#endif

#if _WIN64
#define TARGET_WIN64
#elif __APPLE__ && __MACH__
#define TARGET_MACOSX
#elif __linux__
#define TARGET_LINUX
#else
#define TARGET_UNDEFINED
#endif

#if _MSC_VER
#define STD_FILESYSTEM 1
#else
#define STD_FILESYSTEM 0
#endif

namespace OS
{
namespace Threads
{
using Handle = u64;

Handle Spawn(Task task);
void Join(Handle& handle);
u32 RunningThreadCount(bool bIncludeMain);
u32 MaxConcurrentThreadCount(bool bIncludeMain);
u32 VacantThreadCount();
} // namespace Threads

class EnvData final
{
private:
	std::string m_exePath;
	std::string m_runtimePath;
	std::string m_cwd;
	Map<std::string, std::string> m_vars;
	s32 m_argc = 0;
	char** m_argv = nullptr;

public:
	void SetVars(s32 argc, char** argv, std::initializer_list<std::string_view> runtimeFiles);

	bool HasVar(const std::string& key) const;
	std::optional<std::string> GetVar(const std::string& key) const;
	Pair<s32, char**> OrgVars() const;

	std::string_view ExePath() const;
	std::string_view RuntimePath() const;
	std::string_view CWD() const;

	// Appends "PWD/"
	std::string FullPath(std::string_view relativePath) const;
};

EnvData* Env();

u32 ScreenRefreshRate(u32 assume = 240);
bool IsMainThread();

u64 FileSize(std::string_view path);
bool DoesFileExist(std::string_view path);

bool IsDEBUGGING();
bool IsSHIPPING();
} // namespace OS
