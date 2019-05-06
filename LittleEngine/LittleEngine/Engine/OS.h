#pragma once
#include "Core/StdTypes.h"
#include "SFMLAPI/Windowing/SFWindowData.h"

namespace LittleEngine
{
namespace OS
{
class PlatformData final
{
private:
	SFWindowSize m_maxWindowSize;
	const u32 m_cpuThreadCount;
	s32 m_spareThreadCount = 0;
	s32 m_userWorkerCount = 0;
	s32 m_systemWorkerCount = 0;

	// System Thread Flags
	bool m_bWillCreateRenderThread = false;
	bool m_bWillCreateLoggerThread = false;

public:
	PlatformData();

	u32 TotalThreadCount() const;
	u32 SpareThreadCount() const;
	u32 UserWorkerCount() const;
	u32 SystemWorkerCount() const;
	bool CanCreateSystemThread() const;

	const SFWindowSize& MaxWindowSize() const;

	void SetCreatingRenderThread();
	void SetCreatingLoggerThread();
	void ReleaseLoggerThread();
	void ReleaseRenderThread();
	void SetDesiredWorkerCount(u32 workerCount);
};

PlatformData* Platform();

class EnvData final
{
private:
	String m_exePath;
	Map<String, String> m_vars;

public:
	void SetVars(s32 argc, char** argv);

	bool HasVar(const String& key);
	std::pair<bool, String> GetVar(const String& key);
};

EnvData* Env();

bool IsMainThread();
bool IsDebuggerAttached();
bool IsDEBUGGING();
bool IsSHIPPING();
} // namespace OS
} // namespace LittleEngine
