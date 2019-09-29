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
	String m_exePath;
	String m_runtimePath;
	String m_cwd;
	Map<String, String> m_vars;
	s32 m_argc = 0;
	char** m_argv = nullptr;

public:
	void SetVars(s32 argc, char** argv, InitList<VString> runtimeFiles);

	bool HasVar(const String& key) const;
	std::optional<String> GetVar(const String& key) const;
	Pair<s32, char**> OrgVars() const;

	VString ExePath() const;
	VString RuntimePath() const;
	VString CWD() const;

	// Appends "PWD/"
	String FullPath(VString relativePath) const;
};

EnvData* Env();

u32 ScreenRefreshRate(u32 assume = 240);
bool IsMainThread();
bool IsDebuggerAttached();
void DebugBreak();

u64 FileSize(VString path);
bool DoesFileExist(VString path);

bool IsDEBUGGING();
bool IsSHIPPING();
} // namespace OS
