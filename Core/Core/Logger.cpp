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
using Lock = std::lock_guard<std::mutex>;

LogSeverity g_MinLogSeverity = LogSeverity::Info;
std::function<bool(const char*)> g_OnLogStr;

namespace
{
constexpr size_t CACHE_SIZE = 512;
constexpr size_t BUFFER_SIZE = 4096;

std::mutex _mutex;
char logCache[CACHE_SIZE];
size_t bufferIdx = 0;
char logBuffer[BUFFER_SIZE];
const char* prefixes[5] = {"[H] ", "[D] ", "[I] ", "[W] ", "[E] "};
} // namespace


void LogInternal(const char* pText, u32 severityIndex, va_list argList)
{
	Lock lock(_mutex);
	s32 prefixLength = sprintf(logCache, "%s", prefixes[severityIndex]);
	s32 totalLength = vsnprintf(logCache + prefixLength, BUFFER_SIZE - prefixLength, pText, argList) + prefixLength;
	using namespace std::chrono;
	std::time_t now = system_clock::to_time_t(system_clock::now());
	std::tm ltm;
	localtime_s(&ltm, &now);
	totalLength += snprintf(logCache + totalLength, BUFFER_SIZE - totalLength, " [%02d:%02d:%02d]",
							ltm.tm_hour, ltm.tm_min, ltm.tm_sec);
	strcat_s(logCache, BUFFER_SIZE - totalLength, "\n");
#if _WIN64
	OutputDebugStringA(logCache);
#endif
	std::cout << logCache;
	sprintf(logBuffer + bufferIdx, "%s", logCache);
	bufferIdx = strlen(logBuffer);
	if (g_OnLogStr)
	{
		if (g_OnLogStr(logBuffer))
		{
			memset(logBuffer, 0, BUFFER_SIZE);
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
