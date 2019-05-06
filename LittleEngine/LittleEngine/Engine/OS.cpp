#include "stdafx.h"
#include <thread>
#include "OS.h"
#include "Asserts.h"
#include "Logger.h"
#include "SFMLAPI/Windowing/SFWindow.h"
#if _WIN32
#include <Windows.h>
#endif

namespace LittleEngine
{
namespace
{
std::thread::id mainThreadID;
}

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
	mainThreadID = std::this_thread::get_id();
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

void PlatformData::ReleaseLoggerThread()
{
	m_bWillCreateLoggerThread = false;
	++m_spareThreadCount;
	++m_systemWorkerCount;
}

void PlatformData::ReleaseRenderThread()
{
	m_bWillCreateRenderThread = false;
	++m_spareThreadCount;
	++m_systemWorkerCount;
}

void PlatformData::SetDesiredWorkerCount(u32 workerCount)
{
	if (static_cast<u32>(m_userWorkerCount) > workerCount)
	{
		m_userWorkerCount = workerCount;
	}
}

void EnvData::SetVars(s32 argc, char** argv)
{
	if (argc > 0)
	{
		m_exePath = String(argv[0]);
	}

	String logOut;
	for (s32 i = 1; i < argc; ++i)
	{
		String raw = String(argv[i]);
		if (raw.length() > 2 && raw[0] == '-' && raw[1] == '-')
		{
			String key = raw.substr(2);
			String value;
			if (i < argc - 1)
			{
				value = String(argv[i + 1]);
			}
			if (value.empty())
			{
				logOut += ("[" + key + "], ");
			}
			else
			{
				logOut += ("[" + key + "/" + value + "], ");
			}
			m_vars.emplace(std::move(key), std::move(value));
		}
	}
	LOG_I("[Env] Obtained exe path: %s", m_exePath.c_str());
	if (!logOut.empty())
	{
		logOut = logOut.substr(0, logOut.length() - 2);
		LOG_I("[Env] Obtained env vars: %s", logOut.c_str());
	}
}

bool EnvData::HasVar(const String& key)
{
	return m_vars.find(key) != m_vars.end();
}

std::pair<bool, String> EnvData::GetVar(const String& key)
{
	auto search = m_vars.find(key);
	bool bFound = search != m_vars.end();
	String val = bFound ? search->second : String();
	return std::make_pair(bFound, std::move(val));
}

bool IsMainThread()
{
	return std::this_thread::get_id() == mainThreadID;
}

bool IsDebuggerAttached()
{
#if _MSC_VER
	return IsDebuggerPresent();
#endif
	return false;
}

bool IsDEBUGGING()
{
#if DEBUGGING
	return true;
#endif
	return false;
}

bool IsSHIPPING()
{
#if SHIPPING
	return true;
#endif
	return false;
}

PlatformData* Platform()
{
	static PlatformData platformData;
	return &platformData;
}

EnvData* Env()
{
	static EnvData envData;
	return &envData;
}
} // namespace OS
} // namespace OS
