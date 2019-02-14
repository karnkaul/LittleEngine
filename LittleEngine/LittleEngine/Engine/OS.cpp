#include "stdafx.h"
#include <thread>
#include "OS.h"
#include "Asserts.h"

namespace LittleEngine
{
OS::PlatformData::PlatformData() : cpuThreadCount(std::thread::hardware_concurrency())
{
	/**
	 * Scheme: 
	 * [App Threads]	[Renderer]		[Logger]		[User Workers]
	 *		0				n/a				n/a				n/a
	 *		1				F				F				1
	 *		2				T/F				F/T				1
	 *		3				T				T				1
	 *		n				T				T				n - 2
	 */

	spareThreadCount = cpuThreadCount - 1;		// Subtract main thread
	if (spareThreadCount < 1)
	{
		// Need at least two threads
		Assert(spareThreadCount > 0, "Insufficient threads for application to run!");
		exit(-1);
	}

	switch (spareThreadCount)
	{
	case 1:
	{
		systemWorkerCount = 0;
	}
	break;

	case 2:
	{
		systemWorkerCount = 1;
	}
	break;

	default:
	{
		systemWorkerCount = 2;
	}
	break;
	}
	
	userWorkerCount = spareThreadCount - systemWorkerCount;
}

u32 OS::PlatformData::TotalThreadCount() const
{
	return cpuThreadCount;
}

u32 OS::PlatformData::SpareThreadCount() const
{
	return spareThreadCount;
}

u32 OS::PlatformData::UserWorkerCount() const
{
	return userWorkerCount;
}

u32 OS::PlatformData::SystemWorkerCount() const
{
	return systemWorkerCount;
}

bool OS::PlatformData::CanCreateSystemThread() const
{
	return systemWorkerCount > 0;
}

void OS::PlatformData::SetCreatingRenderThread()
{
	if (systemWorkerCount < 1)
	{
		Assert(systemWorkerCount > 0, "Cannot create Render Thread! No system threads remaining!");
		exit(-1);
	}
	bWillCreateRenderThread = true;
	--spareThreadCount;
	--systemWorkerCount;
}

void OS::PlatformData::SetCreatingLoggerThread()
{
	if (systemWorkerCount < 1)
	{
		Assert(systemWorkerCount > 0, "Cannot create Logger Thread! No system threads remaining!");
		exit(-1);
	}
	bWillCreateLoggerThread = true;
	--spareThreadCount;
	--systemWorkerCount;
}

void OS::PlatformData::SetDesiredWorkerCount(u32 workerCount)
{
	if (static_cast<u32>(userWorkerCount) > workerCount)
		userWorkerCount = workerCount;
}

OS::PlatformData* OS::Platform()
{
	static PlatformData platformData;
	return &platformData;
}
} // namespace LittleEngine
