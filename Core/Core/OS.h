#pragma once
#include "StdTypes.h"
#include "Vector2.h"
#include "Version.h"

namespace OS
{
namespace Threads
{
using Handle = u64;

Handle Spawn(Task task);
void Join(Handle& handle);
u32 GetRunningThreadCount(bool bIncludeMain);
u32 GetMaxConcurrentThreadCount(bool bIncludeMain);
u32 GetVacantThreadCount();
} // namespace Threads

class EnvData final
{
private:
	String m_exePath;
	Map<String, String> m_vars;
	Core::Version m_fileGameVersion;
	Core::Version m_fileEngineVersion;
	s32 m_argc;
	char** m_argv;

public:
	void SetVars(s32 argc, char** argv);

	bool HasVar(const String& key);
	std::pair<bool, String> GetVar(const String& key);
	std::pair<s32, char**> GetOrgVars() const;

	Core::Version GetFileGameVersion() const;
	Core::Version GetFileEngineVersion() const;
};

EnvData* Env();

bool IsMainThread();
bool IsDebuggerAttached();
bool IsDEBUGGING();
bool IsSHIPPING();
} // namespace OS
