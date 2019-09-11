#include <thread>
#include <fstream>
#include "OS.h"

#if defined(TARGET_WIN64)
#if ENABLED(STD_FILESYSTEM)
#include <filesystem>
#endif
#include <Windows.h>
#include <winver.h>
#elif defined(TARGET_LINUX)
#include <cstring>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#endif

#if defined(CXX_LIBC) || defined(TARGET_MACOSX)
#ifdef CXX_LIBC
#include <fcntl.h>
#endif
#include <signal.h>
#include <unistd.h>
#endif

#include "ArchiveReader.h"
#include "Asserts.h"
#include "Logger.h"
#include "Property.h"
#include "Utils.h"

#ifdef DEBUGGING
namespace LittleEngine
{
// To use for conditional breaks, etc
bool g_bDEBUG = false;
} // namespace LittleEngine
#endif

namespace
{
std::thread::id mainThreadID;
#ifdef TARGET_WIN64
const char PATH_DELIM = '\\';
#else
const char PATH_DELIM = '/';
#endif
} // namespace

namespace OS
{
namespace Threads
{
Handle nextHandle = 1;
UMap<Handle, UPtr<std::thread>> threads;

Handle Spawn(Task task)
{
	UPtr<std::thread> uThread = MakeUnique<std::thread>(task);
	Handle handle = nextHandle++;
	threads.emplace(handle, std::move(uThread));
	return handle;
}

void Join(Handle& handle)
{
	if (handle > 0)
	{
		auto iter = threads.find(handle);
		if (iter != threads.end())
		{
			if (iter->second->joinable())
			{
				iter->second->join();
			}
			threads.erase(iter);
		}
		handle = 0;
	}
}

u32 RunningThreadCount(bool bIncludeMain)
{
	return static_cast<u32>(threads.size() + (bIncludeMain ? 1 : 0));
}

u32 MaxConcurrentThreadCount(bool bIncludeMain)
{
	return std::thread::hardware_concurrency() + (bIncludeMain ? 1 : 0);
}

u32 VacantThreadCount()
{
	return MaxConcurrentThreadCount(false) - RunningThreadCount(false);
}
} // namespace Threads

void EnvData::SetVars(s32 argc, char** argv, InitList<const char*> runtimeFiles)
{
#ifdef TARGET_LINUX
	s32 threadStatus = XInitThreads();
	if (threadStatus == 0)
	{
		LOG_E("[OS] ERROR calling XInitThreads()! UB follows.");
	}
#endif
	mainThreadID = std::this_thread::get_id();
	m_argc = argc;
	m_argv = argv;

	if (argc > 0)
	{
		m_exePath = String(argv[0]);
		m_exePath = m_exePath.substr(0, m_exePath.find_last_of(PATH_DELIM));
#ifdef TARGET_MACOSX
		auto bundleIdx = m_exePath.find(".app/Contents/MacOS");
		if (bundleIdx != std::string::npos)
		{
			m_exePath = m_exePath.substr(0, bundleIdx);
			m_exePath = m_exePath.substr(0, m_exePath.find_last_of("/"));
		}
#endif
		Core::ArchiveReader::s_argv0 = argv[0];
	}
#if ENABLED(STD_FILESYSTEM)
	m_cwd = std::filesystem::current_path().string();
#else
	char buf[512];
	char* szPWD = getcwd(buf, sizeof(buf));
	if (szPWD)
	{
		m_cwd = szPWD;
	}
#endif

	for (auto szFilename : runtimeFiles)
	{
		String filename = m_cwd + "/" + szFilename;
		if (std::ifstream(filename.c_str()).good())
		{
			m_runtimePath = m_cwd;
			break;
		}
		filename = m_exePath + "/" + szFilename;
		if (std::ifstream(filename.c_str()).good())
		{
			m_runtimePath = m_exePath;
			break;
		}
	}
	Assert(!m_runtimePath.empty(), "Could not determine Runtime path!");

	for (s32 i = 1; i < argc; ++i)
	{
		String raw(argv[i]);
		if (!raw.empty())
		{
			if (raw[0] == '-')
			{
				raw = raw.substr(1);
			}
			String key, value;
			auto tokens = Strings::Tokenise(raw, '=', {});
			if (!tokens.empty())
			{
				key = std::move(tokens[0]);
				if (tokens.size() > 1)
				{
					value = std::move(tokens[1]);
				}
				if (!key.empty())
				{
					m_vars[std::move(key)] = std::move(value);
				}
			}
		}
	}
	String varsLog;
	for (const auto& kvp : m_vars)
	{
		const auto& key = kvp.first;
		const auto& value = kvp.second;
		if (!key.empty())
		{
			if (value.empty())
			{
				varsLog.append("[").append(key).append("], ");
			}
			else
			{
				varsLog.append("[").append(key).append("/").append(value).append("], ");
			}
		}
	}

	LOG_I("[Env] Runtime path: %s", m_runtimePath.c_str());
	LOG_D("[Env] Exe path: %s", m_exePath.c_str());
	LOG_D("[Env] CWD: %s", m_cwd.c_str());
	if (!varsLog.empty())
	{
		varsLog[varsLog.length() - 2] = 0;
		LOG_I("[Env] Obtained env vars: %s", varsLog.c_str());
	}
}

bool EnvData::HasVar(const String& key) const
{
	return m_vars.find(key) != m_vars.end();
}

std::optional<String> EnvData::GetVar(const String& key) const
{
	std::optional<String> ret;
	auto search = m_vars.find(key);
	if (search != m_vars.end())
	{
		ret = search->second;
	}
	return ret;
}

Pair<s32, char**> EnvData::OrgVars() const
{
	return std::make_pair(m_argc, m_argv);
}

const String& EnvData::ExePath() const
{
	return m_exePath;
}

const String& EnvData::RuntimePath() const
{
	return m_runtimePath;
}

const String& EnvData::CWD() const
{
	return m_cwd;
}

String EnvData::FullPath(const char* szRelativePath) const
{
	String path = m_runtimePath;
	path += "/";
	path += szRelativePath;
	return path;
}

u32 ScreenRefreshRate(u32 assume)
{
	u32 refreshRate = assume;
#if defined(TARGET_WIN64)
	DEVMODE lpDevMode;
	memset(&lpDevMode, 0, sizeof(DEVMODE));
	lpDevMode.dmSize = sizeof(DEVMODE);
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &lpDevMode);
	refreshRate = lpDevMode.dmDisplayFrequency;
#elif defined(TARGET_LINUX)
	s32 num_sizes = 0;
	Display* dpy = XOpenDisplay(NULL);
	Window root = RootWindow(dpy, 0);
	XRRScreenSize* xrrs = XRRSizes(dpy, 0, &num_sizes);
	XRRScreenConfiguration* conf = XRRGetScreenInfo(dpy, root);
	refreshRate = XRRConfigCurrentRate(conf);
#endif
	return refreshRate;
}

bool IsMainThread()
{
	return std::this_thread::get_id() == mainThreadID;
}

bool IsDebuggerAttached()
{
	bool ret = false;
#if defined(TARGET_WIN64)
	ret = IsDebuggerPresent();
#elif defined(TARGET_LINUX)
	char buf[4096];

	const int status_fd = ::open("/proc/self/status", O_RDONLY);
	if (status_fd == -1)
		return false;

	const ssize_t num_read = ::read(status_fd, buf, sizeof(buf) - 1);
	if (num_read <= 0)
		return false;

	buf[num_read] = '\0';
	constexpr char tracerPidString[] = "TracerPid:";
	const auto tracer_pid_ptr = ::strstr(buf, tracerPidString);
	if (!tracer_pid_ptr)
		return false;

	for (const char* characterPtr = tracer_pid_ptr + sizeof(tracerPidString) - 1; characterPtr <= buf + num_read; ++characterPtr)
	{
		if (::isspace(*characterPtr))
			continue;
		else
			ret = ::isdigit(*characterPtr) != 0 && *characterPtr != '0';
	}
#endif
	return ret;
}

void DebugBreak()
{
#if defined(CXX_MS_CRT)
	__debugbreak();
#elif defined(CXX_LIBC) || defined(CXX_LIBSTDC)
#ifdef SIGTRAP
	raise(SIGTRAP);
#else
	raise(SIGILL);
#endif
#endif
}

u64 FileSize(const char* szPath)
{
	u64 size = 0;
#ifdef CXX_MS_CRT
	std::error_code e;
	size = static_cast<u64>(std::filesystem::file_size(szPath, e));
	return e ? 0 : size;
#else
	std::ifstream mySource(szPath, std::ios_base::binary);
	mySource.seekg(0, std::ios_base::end);
	size = static_cast<u64>(mySource.tellg());
#endif
	return size;
}

bool IsDEBUGGING()
{
#ifdef DEBUGGING
	return true;
#else
	return false;
#endif
}

bool IsSHIPPING()
{
#ifdef SHIPPING
	return true;
#else
	return false;
#endif
}

EnvData* Env()
{
	static EnvData envData;
	return &envData;
}
} // namespace OS
