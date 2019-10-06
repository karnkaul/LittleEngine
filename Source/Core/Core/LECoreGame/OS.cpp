#include <atomic>
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include "OS.h"
#if defined(TARGET_WIN64)
#if defined(STD_FILESYSTEM) && STD_FILESYSTEM
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
#include "LECoreUtils/FileRW.h"
#include "LECoreUtils/Logger.h"
#include "LECoreUtils/Utils.h"
#include "ArchiveReader.h"
#include "Property.h"

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

class FileLogger final
{
private:
	using Lock = std::lock_guard<std::mutex>;

private:
	const std::string m_extension = ".log";
	std::string m_filename;
	std::string m_cache;
	std::unique_ptr<Core::FileRW> m_uWriter;
	std::atomic<bool> m_bStopLogging;
	std::mutex m_cacheMutex;
	OS::Threads::Handle m_threadHandle;

public:
	FileLogger(std::string filename, u8 backupCount, std::string header = "");
	~FileLogger();

	void OnLogStr(std::string&& capture);

private:
	void Async_StartLogging();
	void RenameOldFiles(u16 countToKeep);
};

std::tm* TM(const std::time_t& time)
{
#if _MSC_VER
	static std::tm tm;
	localtime_s(&tm, &time);
	return &tm;
#else
	return localtime(&time);
#endif
}

std::string Prologue(std::string header)
{
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	auto pTM = TM(now);
	static char buffer[256];
	std::strftime(buffer, 256, "%a %F %T", pTM);
	std::string ret(buffer);
	ret += " ";
	ret += std::move(header);
	ret += "\n";
	return ret;
}

FileLogger::FileLogger(std::string filename, u8 backupCount, std::string header) : m_filename(std::move(filename))
{
	RenameOldFiles(backupCount);
	m_uWriter = std::make_unique<Core::FileRW>(m_filename + m_extension);
	m_uWriter->Write(Prologue(std::move(header)));
	m_bStopLogging.store(false, std::memory_order_relaxed);
	m_threadHandle = OS::Threads::Spawn([&]() { Async_StartLogging(); });
	Core::g_onLogStr = [this](auto logStr) { OnLogStr(std::move(logStr)); };
}

FileLogger::~FileLogger()
{
	Core::g_onLogStr = nullptr;
	LOG_D("Logging terminated");
	// Freeze m_cache and terminate thread
	m_bStopLogging.store(true);
	OS::Threads::Join(m_threadHandle);
}

void FileLogger::OnLogStr(std::string&& capture)
{
	Lock lock(m_cacheMutex);
	m_cache += std::move(capture);
	capture.clear();
}

void FileLogger::Async_StartLogging()
{
	while (!m_bStopLogging.load(std::memory_order_relaxed))
	{
		std::string toWrite;
		{
			Lock lock(m_cacheMutex);
			toWrite = std::move(m_cache);
		}
		m_uWriter->Append(std::move(toWrite));
		std::this_thread::yield();
	}

	{
		Lock lock(m_cacheMutex);
		if (!m_cache.empty())
		{
			m_uWriter->Append(std::move(m_cache));
		}
	}
	m_uWriter = nullptr;
}

void FileLogger::RenameOldFiles(u16 countToKeep)
{
	// Make room for oldest backup
	std::string oldest = m_filename + "_bak_" + Strings::ToString(countToKeep) + m_extension;
	if (std::ifstream(oldest))
	{
		remove(oldest.c_str());
	}
	--countToKeep;
	s32 success = 0;

	// Rename old backups
	while (countToKeep > 0)
	{
		std::string from = m_filename + "_bak_" + Strings::ToString(countToKeep) + m_extension;
		std::string to = m_filename + "_bak_" + Strings::ToString(countToKeep + 1) + m_extension;
		if (std::ifstream(from))
		{
			if (std::ifstream(to))
			{
				remove(to.c_str());
			}
			success += std::rename(from.c_str(), to.c_str());
		}
		--countToKeep;
	}

	// Rename last log file
	std::string from = m_filename + m_extension;
	std::string to = m_filename + "_bak_" + Strings::ToString(1) + m_extension;
	if (std::ifstream(from))
	{
		success += std::rename(from.c_str(), to.c_str());
	}

	if (success != 0)
	{
		LOG_W("[AsyncFileLogger] Could not rename all old log files");
	}
}

std::unique_ptr<FileLogger> uFileLogger;
} // namespace

namespace OS
{
namespace Threads
{
Handle nextHandle = 1;
std::unordered_map<Handle, std::unique_ptr<std::thread>> threads;

Handle Spawn(Task task)
{
	auto uThread = std::make_unique<std::thread>(task);
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

void EnvData::SetVars(s32 argc, char** argv, std::initializer_list<std::string_view> runtimeFiles)
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
		m_exePath = std::string(argv[0]);
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
#if defined(STD_FILESYSTEM) && STD_FILESYSTEM
	m_cwd = std::filesystem::current_path().string();
#else
	char buf[512];
	char* szPWD = getcwd(buf, sizeof(buf));
	if (szPWD)
	{
		m_cwd = szPWD;
	}
#endif

	for (auto file : runtimeFiles)
	{
		std::string filename = m_cwd;
		filename += "/";
		filename += file;
		if (std::ifstream(filename.c_str()).good())
		{
			m_runtimePath = m_cwd;
			break;
		}
		filename = m_exePath;
		filename += "/";
		filename += file;
		if (std::ifstream(filename.c_str()).good())
		{
			m_runtimePath = m_exePath;
			break;
		}
	}
	Assert(!m_runtimePath.empty(), "Could not determine Runtime path!");

	for (s32 i = 1; i < argc; ++i)
	{
		std::string raw(argv[i]);
		if (!raw.empty())
		{
			if (raw[0] == '-')
			{
				raw = raw.substr(1);
			}
			std::string key, value;
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
	std::string varsLog;
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

bool EnvData::HasVar(const std::string& key) const
{
	return m_vars.find(key) != m_vars.end();
}

std::optional<std::string> EnvData::GetVar(const std::string& key) const
{
	std::optional<std::string> ret;
	auto search = m_vars.find(key);
	if (search != m_vars.end())
	{
		ret = search->second;
	}
	return ret;
}

std::pair<s32, char**> EnvData::OrgVars() const
{
	return std::make_pair(m_argc, m_argv);
}

std::string_view EnvData::ExePath() const
{
	return m_exePath;
}

std::string_view EnvData::RuntimePath() const
{
	return m_runtimePath;
}

std::string_view EnvData::CWD() const
{
	return m_cwd;
}

std::string EnvData::FullPath(std::string_view relativePath) const
{
	std::string path = m_runtimePath;
	path += "/";
	path += relativePath;
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

	const auto status_fd = ::open("/proc/self/status", O_RDONLY);
	if (status_fd == -1)
		return false;

	const auto num_read = ::read(status_fd, buf, sizeof(buf) - 1);
	if (num_read <= 0)
		return false;

	buf[num_read] = '\0';
	constexpr char tracerPid[] = "TracerPid:";
	const auto tracer_pid_ptr = ::strstr(buf, tracerPid);
	if (!tracer_pid_ptr)
		return false;

	for (const char* characterPtr = tracer_pid_ptr + sizeof(tracerPid) - 1; characterPtr <= buf + num_read; ++characterPtr)
	{
		if (::isspace(*characterPtr))
		{
			continue;
		}
		else
		{
			ret = ::isdigit(*characterPtr) != 0 && *characterPtr != '0';
		}
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

u64 FileSize(std::string_view path)
{
	u64 size = 0;
	if (DoesFileExist(path))
	{
#ifdef CXX_MS_CRT
		std::error_code e;
		size = static_cast<u64>(std::filesystem::file_size(path, e));
		return e ? 0 : size;
#else
		std::ifstream mySource(path.data(), std::ios_base::binary);
		mySource.seekg(0, std::ios_base::end);
		size = static_cast<u64>(mySource.tellg());
#endif
	}
	return size;
}

bool DoesFileExist(std::string_view path)
{
	return std::ifstream(path.data()).good();
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

void StartFileLogging(std::string path, u8 backupCount, std::string header)
{
	uFileLogger = std::make_unique<FileLogger>(std::move(path), backupCount, std::move(header));
}

void StopFileLogging()
{
	uFileLogger = nullptr;
}
} // namespace OS
