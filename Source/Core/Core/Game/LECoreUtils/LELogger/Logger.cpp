#include <array>
#include <atomic>
#include <ctime>
#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <mutex>
#include <string>
#include <time.h>
#include <thread>
#include "Logger.h"
#if _MSC_VER
#include "Windows.h"
#endif

namespace LE
{
using Lock = std::lock_guard<std::mutex>;

LE::LogSeverity g_MinLogSeverity = LE::LogSeverity::Info;
std::function<void(std::string logStr)> g_onLogStr;

namespace
{
constexpr size_t LOG_BUFFER_SIZE = 4096;
constexpr size_t CACHE_SIZE = 512;

bool bInit = false;
std::mutex _mutex;
std::string cache;
std::string buffer;
std::array<std::string_view, 5> prefixes = {"[D] ", "[I] ", "[W] ", "[E] "};

std::unordered_map<LE::LogSeverity, std::string_view> severityMap = {{LE::LogSeverity::Error, "Error"},
																	 {LE::LogSeverity::Warning, "Warning"},
																	 {LE::LogSeverity::Info, "Info"},
																	 {LE::LogSeverity::Debug, "Debug"}};

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

void LogInternal(const char* pText, uint32_t severityIndex, va_list argList)
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
	if (g_onLogStr)
	{
		g_onLogStr(std::move(buffer));
	}
}
} // namespace

void Log(LE::LogSeverity severity, const char* szText, ...)
{
	auto severityIndex = static_cast<int>(severity);
	if (severityIndex < static_cast<int>(LE::g_MinLogSeverity))
	{
		return;
	}

	va_list argList;
	va_start(argList, szText);
	LogInternal(szText, static_cast<uint32_t>(severityIndex), argList);
	va_end(argList);
}

std::string_view ParseSeverity(LogSeverity severity)
{
	return severityMap[severity];
}

LogSeverity ParseSeverity(std::string_view serialised)
{
	for (const auto& severity : severityMap)
	{
		if (severity.second == serialised)
		{
			return severity.first;
		}
	}
	return LogSeverity::Info;
}
} // namespace LE
