#include <atomic>
#include <ctime>
#include <cstdarg>
#include <cstring>
#include <fstream>
#include <iostream>
#include <cstdio>
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
LogSeverity g_MinLogSeverity = LogSeverity::Info;

namespace
{
constexpr size_t LOG_BUFFER_SIZE = 4096;
constexpr size_t CACHE_SIZE = 512;

bool bInit = false;
std::mutex _mutex;
Array<char, CACHE_SIZE> logCache;
size_t bufferIdx = 0;
using LogArr = Array<char, LOG_BUFFER_SIZE>;
LogArr logBuffer;
Array<const char*, 5> prefixes = {"[H] ", "[D] ", "[I] ", "[W] ", "[E] "};

UMap<Core::LogSeverity, String> severityMap = {{LogSeverity::Error, "Error"},
											   {LogSeverity::Warning, "Warning"},
											   {LogSeverity::Info, "Info"},
											   {LogSeverity::Debug, "Debug"},
											   {LogSeverity::HOT, "HOT"}};

class FileLogger final
{
private:
	const String m_extension = ".log";
	String m_filename;
	String m_cache;
	UPtr<class FileRW> m_uWriter;
	std::atomic<bool> m_bStopLogging;
	std::mutex m_cacheMutex;
	OS::Threads::Handle m_threadHandle;

public:
	FileLogger(String filename, u8 backupCount, String header = "");
	~FileLogger();

	bool OnLogStr(const LogArr& cache);

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

String Prologue(String header)
{
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	auto pTM = TM(now);
	static char buffer[256];
	std::strftime(buffer, 256, "%a %F %T", pTM);
	String ret(buffer);
	ret += (" " + std::move(header) + "\n");
	return ret;
}

FileLogger::FileLogger(String filename, u8 backupCount, String header) : m_filename(std::move(filename))
{
	RenameOldFiles(backupCount);
	m_uWriter = MakeUnique<FileRW>(m_filename + m_extension);
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

bool FileLogger::OnLogStr(const LogArr& cache)
{
	if (!m_bStopLogging.load(std::memory_order_relaxed))
	{
		Lock lock(m_cacheMutex);
		m_cache += std::string(cache.data());
		return true;
	}
	return false;
}

void FileLogger::Async_StartLogging()
{
	while (!m_bStopLogging.load(std::memory_order_relaxed))
	{
		String toWrite;
		{
			Lock lock(m_cacheMutex);
			toWrite = std::move(m_cache);
		}
		m_uWriter->Append(toWrite);
		std::this_thread::yield();
	}

	// m_cache is now read-only from main thread, no lock required
	if (!m_cache.empty())
	{
		m_uWriter->Append(std::move(m_cache));
	}
	m_uWriter = nullptr;
}

void FileLogger::RenameOldFiles(u16 countToKeep)
{
	// Make room for oldest backup
	String oldest = m_filename + "_bak_" + Strings::ToString(countToKeep) + m_extension;
	if (std::ifstream(oldest))
	{
		remove(oldest.c_str());
	}
	--countToKeep;
	s32 success = 0;

	// Rename old backups
	while (countToKeep > 0)
	{
		String from = m_filename + "_bak_" + Strings::ToString(countToKeep) + m_extension;
		String to = m_filename + "_bak_" + Strings::ToString(countToKeep + 1) + m_extension;
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
	String from = m_filename + m_extension;
	String to = m_filename + "_bak_" + Strings::ToString(1) + m_extension;
	if (std::ifstream(from))
	{
		success += std::rename(from.c_str(), to.c_str());
	}

	if (success != 0)
	{
		LOG_W("[AsyncFileLogger] Could not rename all old log files");
	}
}

UPtr<FileLogger> uFileLogger;
} // namespace

void LogInternal(const char* pText, u32 severityIndex, va_list argList)
{
	if (!bInit)
	{
		std::memset(logCache.data(), 0, logCache.size());
		std::memset(logBuffer.data(), 0, logBuffer.size());
		bInit = true;
	}
	Lock lock(_mutex);
	auto prefixLength = static_cast<size_t>(SPRINTF(logCache.data(), logCache.size(), "%s", prefixes[severityIndex]));
	auto totalLength =
		static_cast<size_t>(vsnprintf(logCache.data() + prefixLength, logCache.size() - prefixLength, pText, argList)) + prefixLength;
	using namespace std::chrono;
	std::time_t now = system_clock::to_time_t(system_clock::now());
	auto pTM = TM(now);
	totalLength += static_cast<size_t>(snprintf(logCache.data() + totalLength, logCache.size() - totalLength, " [%02d:%02d:%02d]",
												pTM->tm_hour, pTM->tm_min, pTM->tm_sec));
	STRCAT(logCache.data(), logCache.size(), "\n");
#if _MSC_VER
	OutputDebugStringA(logCache.data());
#endif
	std::cout << logCache.data();
	SPRINTF(logBuffer.data() + bufferIdx, logBuffer.size() - bufferIdx, "%s", logCache.data());
	bufferIdx = strlen(logBuffer.data());
	if (uFileLogger)
	{
		if (uFileLogger->OnLogStr(logBuffer))
		{
			memset(logBuffer.data(), 0, logBuffer.size());
			bufferIdx = 0;
		}
	}
}

void Log(LogSeverity severity, const char* pText, ...)
{
	u32 severityIndex = static_cast<u32>(severity);
	if (severityIndex < static_cast<u32>(g_MinLogSeverity))
	{
		return;
	}

	va_list argList;
	va_start(argList, pText);
	LogInternal(pText, severityIndex, argList);
	va_end(argList);
}

String ParseLogSeverity(LogSeverity severity)
{
	return severityMap[severity];
}

LogSeverity ParseLogSeverity(const String& serialised)
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

void StartFileLogging(String path, u8 backupCount, String header)
{
	uFileLogger = MakeUnique<FileLogger>(std::move(path), backupCount, std::move(header));
}

void StopFileLogging()
{
	uFileLogger = nullptr;
}
} // namespace Core
