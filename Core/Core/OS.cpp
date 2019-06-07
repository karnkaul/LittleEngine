#include "stdafx.h"
#include <thread>
#include "OS.h"
#include "ArchiveReader.h"
#include "Asserts.h"
#include "Logger.h"
#include "Utils.h"
#if _WIN64
#include <Windows.h>
#include <winver.h>
#endif

using Version = Core::Version;

namespace
{
std::thread::id mainThreadID;

#if _WIN64
#pragma comment(lib, "Version.lib")
std::pair<Version, Version> ExtractGameAndEngineVersions(const String& exePath)
{
	UINT size = 0;
	LPVOID lpBuffer = nullptr;
	DWORD verSize = GetFileVersionInfoSizeA(exePath.c_str(), nullptr);

	Version fileGameVersion;
	Version fileEngineVersion;

	if (verSize)
	{
		Vec<DWORD> verData(verSize);
		if (GetFileVersionInfoA(exePath.c_str(), 0, verSize, verData.data()))
		{
			if (VerQueryValueA(verData.data(), "\\", &lpBuffer, &size))
			{
				if (size)
				{
					VS_FIXEDFILEINFO* verInfo = reinterpret_cast<VS_FIXEDFILEINFO*>(lpBuffer);
					if (verInfo->dwSignature == 0xfeef04bd)
					{
						s32 major = verInfo->dwFileVersionMS >> 16 & 0xffff;
						s32 minor = verInfo->dwFileVersionMS & 0xffff;
						s32 patch = verInfo->dwFileVersionLS >> 16 & 0xffff;
						s32 pre = verInfo->dwFileVersionLS & 0xffff;
						fileEngineVersion = Version(major, minor, patch, pre);

						major = verInfo->dwProductVersionMS >> 16 & 0xffff;
						minor = verInfo->dwProductVersionMS & 0xffff;
						patch = verInfo->dwProductVersionLS >> 16 & 0xffff;
						pre = verInfo->dwProductVersionLS & 0xffff;
						fileGameVersion = Version(major, minor, patch, pre);
					}
				}
			}
		}
	}
	return std::make_pair(fileGameVersion, fileEngineVersion);
}
#endif
} // namespace

namespace OS
{
namespace Threads
{
struct Thread
{
	UPtr<std::thread> uThread;
	Handle handle;
};

Handle nextHandle = 1;
Vec<Thread> threads;

Handle Spawn(Task task)
{
	UPtr<std::thread> uThread = MakeUnique<std::thread>(task);
	Handle handle = nextHandle++;
	Thread thread = {std::move(uThread), handle};
	threads.emplace_back(std::move(thread));
	return handle;
}

void Join(Handle& handle)
{
	if (handle > 0)
	{
		auto iter = std::find_if(threads.begin(), threads.end(),
								 [handle](const Thread& thread) { return thread.handle == handle; });
		if (iter != threads.end())
		{
			if (iter->uThread->joinable())
			{
				iter->uThread->join();
			}
			Core::RemoveIf<Thread>(threads,
							 [handle](const Thread& thread) { return thread.handle == handle; });
		}
		handle = 0;
	}
}

u32 GetRunningThreadCount(bool bIncludeMain)
{
	return threads.size() + (bIncludeMain ? 1 : 0);
}

u32 GetMaxConcurrentThreadCount(bool bIncludeMain)
{
	return std::thread::hardware_concurrency() + (bIncludeMain ? 1 : 0);
}

u32 GetVacantThreadCount()
{
	return GetMaxConcurrentThreadCount(false) - GetRunningThreadCount(false);
}
}
void EnvData::SetVars(s32 argc, char** argv)
{
	mainThreadID = std::this_thread::get_id();

	m_argc = argc;
	m_argv = argv;

	if (argc > 0)
	{
		m_exePath = String(argv[0]);
		Core::ArchiveReader::s_argv0 = argv[0];
#if _WIN64
		auto versions = ExtractGameAndEngineVersions(m_exePath);
		m_fileGameVersion = versions.first;
		m_fileEngineVersion = versions.second;
#endif
	}

	String logOut;
	for (s32 i = 1; i < argc; ++i)
	{
		String raw = String(argv[i]);
		if (raw.length() > 2)
		{
			String key = (raw[0] == '-' && raw[1] == '-') ? raw.substr(2) : raw;
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

std::pair<s32, char**> EnvData::GetOrgVars() const
{
	return std::make_pair(m_argc, m_argv);
}

Version EnvData::GetFileGameVersion() const
{
	return m_fileGameVersion;
}

Version EnvData::GetFileEngineVersion() const
{
	return m_fileEngineVersion;
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

EnvData* Env()
{
	static EnvData envData;
	return &envData;
}
} // namespace OS
