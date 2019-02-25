#include "stdafx.h"
#include <thread>
#include "OS.h"
#include "Asserts.h"
#include "SFMLAPI/Windowing/SFWindow.h"

namespace LittleEngine
{
namespace OS
{
PlatformData::PlatformData() : m_cpuThreadCount(std::thread::hardware_concurrency())
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

	m_spareThreadCount = m_cpuThreadCount - 1; // Subtract main thread
	if (m_spareThreadCount < 1)
	{
		// Need at least two threads
		Assert(m_spareThreadCount > 0, "Insufficient threads for application to run!");
		exit(-1);
	}

	switch (m_spareThreadCount)
	{
	case 1:
	{
		m_systemWorkerCount = 0;
	}
	break;

	case 2:
	{
		m_systemWorkerCount = 1;
	}
	break;

	default:
	{
		m_systemWorkerCount = 2;
	}
	break;
	}

	m_userWorkerCount = m_spareThreadCount - m_systemWorkerCount;
	m_maxWindowSize = SFWindow::GetMaxWindowSize();
}

u32 PlatformData::TotalThreadCount() const
{
	return m_cpuThreadCount;
}

u32 PlatformData::SpareThreadCount() const
{
	return m_spareThreadCount;
}

u32 PlatformData::UserWorkerCount() const
{
	return m_userWorkerCount;
}

u32 PlatformData::SystemWorkerCount() const
{
	return m_systemWorkerCount;
}

bool PlatformData::CanCreateSystemThread() const
{
	return m_systemWorkerCount > 0;
}

const SFWindowSize& PlatformData::MaxWindowSize() const
{
	return m_maxWindowSize;
}

void PlatformData::SetCreatingRenderThread()
{
	if (m_systemWorkerCount < 1)
	{
		Assert(m_systemWorkerCount > 0,
			   "Cannot create Render Thread! No system threads remaining!");
		exit(-1);
	}
	m_bWillCreateRenderThread = true;
	--m_spareThreadCount;
	--m_systemWorkerCount;
}

void PlatformData::SetCreatingLoggerThread()
{
	if (m_systemWorkerCount < 1)
	{
		Assert(m_systemWorkerCount > 0,
			   "Cannot create Logger Thread! No system threads remaining!");
		exit(-1);
	}
	m_bWillCreateLoggerThread = true;
	--m_spareThreadCount;
	--m_systemWorkerCount;
}

void PlatformData::SetDesiredWorkerCount(u32 workerCount)
{
	if (static_cast<u32>(m_userWorkerCount) > workerCount)
		m_userWorkerCount = workerCount;
}

PlatformData* Platform()
{
	static PlatformData platformData;
	return &platformData;
}
} // namespace OS
} // namespace LittleEngine
