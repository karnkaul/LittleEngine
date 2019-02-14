#pragma once
#include "StdTypes.h"

namespace LittleEngine
{
namespace OS
{
struct PlatformData final
{
private:
	const u32 cpuThreadCount;
	s32 spareThreadCount;
	s32 userWorkerCount;
	s32 systemWorkerCount;

	// System Thread Flags
	bool bWillCreateRenderThread;
	bool bWillCreateLoggerThread;

public:
	PlatformData();

	u32 TotalThreadCount() const;
	u32 SpareThreadCount() const;
	u32 UserWorkerCount() const;
	u32 SystemWorkerCount() const;
	bool CanCreateSystemThread() const;
	
	void SetCreatingRenderThread();
	void SetCreatingLoggerThread();
	void SetDesiredWorkerCount(u32 workerCount);
};

PlatformData* Platform();
};
} // namespace LittleEngine
