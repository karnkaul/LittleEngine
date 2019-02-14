#pragma once
#include "StdTypes.h"
#include <ostream>
#include <sstream>

#if DEBUGGING
#define LogD(x) Core::LogOutput(x, Core::LogSeverity::Debug)
#define LogI(x) Core::LogOutput(x, Core::LogSeverity::Info)
#define LogW(x) Core::LogOutput(x, Core::LogSeverity::Warning)
#define LogE(x) Core::LogOutput(x, Core::LogSeverity::Error)
#else
#define LogD(x)
#define LogI(x)
#define LogW(x)
#define LogE(x)
#endif

namespace Core
{
enum class LogSeverity
{
	HOT = -1,
	Debug = 0,
	Info = 1,
	Warning = 2,
	Error = 3
};

extern Function(void(const String&)) g_OnLogCallback;
extern LogSeverity g_MinLogSeverity;

void LogOutput(StringStream& stream, LogSeverity = LogSeverity::Info);
void LogOutput(const String& str, LogSeverity severity = LogSeverity::Info);
} // namespace Core
