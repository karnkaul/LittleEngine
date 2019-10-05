#include <array>
#include <atomic>
#include <ctime>
#include <cstdarg>
#include <cstring>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <mutex>
#include <string>
#include <time.h>
#include <thread>
#include "FileRW.h"
#include "Logger.h"
#include "OS.h"
#include "Utils.h"
#ifdef TARGET_WIN64
#include "Windows.h"
#endif

namespace Core
{
using Lock = std::lock_guard<std::mutex>;

LogSeverity g_MinLogSeverity = LogSeverity::Info;

namespace
{
constexpr size_t LOG_BUFFER_SIZE = 4096;
constexpr size_t CACHE_SIZE = 512;

bool bInit = false;
std::mutex _mutex;
std::string cache;
std::string buffer;
std::array<std::string_view, 5> prefixes = {"[H] ", "[D] ", "[I] ", "[W] ", "[E] "};

std::unordered_map<Core::LogSeverity, std::string_view> severityMap = {{LogSeverity::Error, "Error"},
												{LogSeverity::Warning, "Warning"},
												{LogSeverity::Info, "Info"},
												{LogSeverity::Debug, "Debug"},
												{LogSeverity::HOT, "HOT"}};

class FileLogger final
{
private:
	const std::string m_extension = ".log";
	std::string m_filename;
	std::string m_cache;
	std::unique_ptr<class FileRW> m_uWriter;
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
	m_uWriter = std::make_unique<FileRW>(m_filename + m_extension);
	m_uWriter->Write(Prologue(std::move(header)));
	m_bStopLogging.store(false, std::memory_order_relaxed);
	m_threadHandle = OS::Threads::Spawn([&]() { Async_StartLogging(); });
}

FileLogger::~FileLogger()
{
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

void LogInternal(const char* pText, u32 severityIndex, va_list argList)
{
	static std::array<char, CACHE_SIZE> cacheStr;
	if (!bInit)
	{
		cache.reserve(CACHE_SIZE);
		buffer.reserve(LOG_BUFFER_SIZE);
		std::memset(cacheStr.data(), 0, cacheStr.size());
		bInit = true;
	}
	Lock lock(_mutex);
	cache.clear();
	cache += prefixes[severityIndex];
	vsnprintf(cacheStr.data(), cacheStr.size(), pText, argList);
	cache += std::string(cacheStr.data());
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	auto pTM = TM(now);
	snprintf(cacheStr.data(), cacheStr.size(), " [%02d:%02d:%02d]", pTM->tm_hour, pTM->tm_min, pTM->tm_sec);
	cache += std::string(cacheStr.data());
	std::cout << cache << std::endl;

	cache += "\n";
#if _MSC_VER
	OutputDebugStringA(cache.data());
#endif
	buffer += cache;
	if (uFileLogger)
	{
		uFileLogger->OnLogStr(std::move(buffer));
	}
}

void Log(LogSeverity severity, const char* szText, ...)
{
	u32 severityIndex = static_cast<u32>(severity);
	if (severityIndex < static_cast<u32>(g_MinLogSeverity))
	{
		return;
	}

	va_list argList;
	va_start(argList, szText);
	LogInternal(szText, severityIndex, argList);
	va_end(argList);
}

std::string_view ParseLogSeverity(LogSeverity severity)
{
	return severityMap[severity];
}

LogSeverity ParseLogSeverity(std::string_view serialised)
{
	for (const auto& severity : severityMap)
	{
		if (severity.second == serialised)
		{
			return severity.first;
		}
	}
	return Core::LogSeverity::Info;
}

void StartFileLogging(std::string path, u8 backupCount, std::string header)
{
	uFileLogger = std::make_unique<FileLogger>(std::move(path), backupCount, std::move(header));
}

void StopFileLogging()
{
	uFileLogger = nullptr;
}
} // namespace Core
