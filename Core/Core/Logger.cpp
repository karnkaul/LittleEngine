#include "stdafx.h"
#include <ctime>
#include <iostream>
#include <mutex>
#if _WIN64
#include "Windows.h"
#endif
#include "Logger.h"

namespace Core
{
LogSeverity g_MinLogSeverity = LogSeverity::Info;
std::function<bool(LogArr&)> g_OnLogStr;

namespace
{
constexpr size_t CACHE_SIZE = 512;

std::mutex _mutex;
Array<char, CACHE_SIZE> logCache;
size_t bufferIdx = 0;
LogArr logBuffer;
Array<const char*, 5> prefixes = {"[H] ", "[D] ", "[I] ", "[W] ", "[E] "};
} // namespace


void LogInternal(const char* pText, u32 severityIndex, va_list argList)
{
	Lock lock(_mutex);
	s32 prefixLength = sprintf(logCache.data(), "%s", prefixes[severityIndex]);
	s32 totalLength = vsnprintf(logCache.data() + prefixLength, LOG_BUFFER_SIZE - prefixLength, pText, argList) + prefixLength;
	using namespace std::chrono;
	std::time_t now = system_clock::to_time_t(system_clock::now());
	std::tm ltm;
	localtime_s(&ltm, &now);
	totalLength += snprintf(logCache.data() + totalLength, LOG_BUFFER_SIZE - totalLength, " [%02d:%02d:%02d]",
							ltm.tm_hour, ltm.tm_min, ltm.tm_sec);
	strcat_s(logCache.data(), LOG_BUFFER_SIZE - totalLength, "\n");
#if _WIN64
	OutputDebugStringA(logCache.data());
#endif
	std::cout << logCache.data();
	sprintf(logBuffer.data() + bufferIdx, "%s", logCache.data());
	bufferIdx = strlen(logBuffer.data());
	if (g_OnLogStr)
	{
		if (g_OnLogStr(logBuffer))
		{
			memset(logBuffer.data(), 0, LOG_BUFFER_SIZE);
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
} // namespace Core
