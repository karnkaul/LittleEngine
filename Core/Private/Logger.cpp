#include "stdafx.h"
#include <iostream>
#include <mutex>
#if _WIN32
#include "Windows.h"
#endif
#include "Logger.h"

namespace Core
{
using Lock = std::lock_guard<std::mutex>;

LogSeverity g_MinLogSeverity = LogSeverity::Info;
Function(void(const String&)) g_OnLogCallback = nullptr;

namespace
{
constexpr size_t BUFFER_SIZE = 2048;

std::mutex _mutex;
char logBuffer[BUFFER_SIZE];
const char* prefixes[5] = {"[H] ", "[D] ", "[I] ", "[W] ", "[E] "};
} // namespace

Function(void(const char*)) g_OnLogStr;

void LogInternal(const char* pText, u32 severityIndex, va_list argList)
{
	Lock lock(_mutex);
	s32 prefixLength = sprintf(logBuffer, "%s", prefixes[severityIndex]);
	s32 totalLength = vsnprintf(logBuffer + prefixLength, BUFFER_SIZE - prefixLength, pText, argList);
	strcat_s(logBuffer, BUFFER_SIZE - totalLength, "\n");
#if _WIN32
	OutputDebugStringA(logBuffer);
#endif
	std::cout << logBuffer;
	if (g_OnLogStr)
		g_OnLogStr(logBuffer);
}

void Log(LogSeverity severity, const char* pText, ...)
{
	u32 severityIndex = static_cast<u32>(severity);
	if (severityIndex < static_cast<u32>(g_MinLogSeverity))
		return;

	va_list argList;
	va_start(argList, pText);
	LogInternal(pText, severityIndex, argList);
	va_end(argList);
}
} // namespace Core
