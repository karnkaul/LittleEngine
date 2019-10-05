#pragma once
#include <functional>
#include "StdTypes.h"

#define LOG_SEVERITY(msg, severity, ...) Core::Log(Core::LogSeverity::severity, msg, ##__VA_ARGS__)
#define LOG_E(x, ...) LOG_SEVERITY(x, Error, ##__VA_ARGS__)
#define LOG_W(x, ...) LOG_SEVERITY(x, Warning, ##__VA_ARGS__)
#define LOG_I(x, ...) LOG_SEVERITY(x, Info, ##__VA_ARGS__)
#define LOGIF_E(bCond, x, ...) if (bCond) { LOG_SEVERITY(x, Error, ##__VA_ARGS__); }
#define LOGIF_W(bCond, x, ...) if (bCond) { LOG_SEVERITY(x, Warning, ##__VA_ARGS__); }
#define LOGIF_I(bCond, x, ...) if (bCond) { LOG_SEVERITY(x, Info, ##__VA_ARGS__); }

#if defined(DEBUG_LOGGING) && DEBUG_LOGGING
#define LOG_D(x, ...) LOG_SEVERITY(x, Debug, ##__VA_ARGS__)
#define LOG_H(x, ...) LOG_SEVERITY(x, HOT, ##__VA_ARGS__)
#define LOGIF_D(bCond, x, ...) if (bCond) { LOG_SEVERITY(x, Debug, ##__VA_ARGS__); }
#else
#define LOG_D(x, ...)
#define LOG_H(x, ...)
#define LOGIF_D(bCond, x, ...)
#endif

namespace Core
{
enum class LogSeverity : u8
{
	HOT = 0,
	Debug,
	Info,
	Warning,
	Error
};

extern LogSeverity g_MinLogSeverity;

void Log(LogSeverity severity, const char* szText, ...);

std::string_view ParseLogSeverity(LogSeverity severity);
LogSeverity ParseLogSeverity(std::string_view serialised);

void StartFileLogging(std::string path, u8 backupCount, std::string header);
void StopFileLogging();
} // namespace Core
