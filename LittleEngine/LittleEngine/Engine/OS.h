#pragma once
#include "StdTypes.h"
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
	void SetDesiredWorkerCount(u32 workerCount);
};

PlatformData* Platform();

bool IsMainThread();
} // namespace OS
} // namespace LittleEngine
