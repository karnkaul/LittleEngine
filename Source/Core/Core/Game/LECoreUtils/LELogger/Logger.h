/* 
 * == LittleEngine Logger ==
 *   Copyright 2019 Karn Kaul
 * Features:
 *   - Thread-safe
 *   - Optionnal callback for file logging / in-app etc
 *   - Severity prefix and date suffix
 *   - Severity filter
 *   - Logs to VS Output Window also
 * Usage:
 *   - Define `DEBUG_LOGGING 1` to enable `LOG_D`
 *   - Include this file and call `LOG_X(message, format)` where `X = { E, W, I }` for Error, Warning, Info
 *   - Set `g_MinLogSeverity` to set the minimum severity threshold
 */

#pragma once
#include <functional>
#include <string>

#define LOG_SEVERITY(msg, severity, ...) LE::Log(LE::LogSeverity::severity, msg, ##__VA_ARGS__)
#define LOG_E(x, ...) LOG_SEVERITY(x, Error, ##__VA_ARGS__)
#define LOG_W(x, ...) LOG_SEVERITY(x, Warning, ##__VA_ARGS__)
#define LOG_I(x, ...) LOG_SEVERITY(x, Info, ##__VA_ARGS__)
#define LOGIF_E(bCond, x, ...) if (bCond) { LOG_SEVERITY(x, Error, ##__VA_ARGS__); }
#define LOGIF_W(bCond, x, ...) if (bCond) { LOG_SEVERITY(x, Warning, ##__VA_ARGS__); }
#define LOGIF_I(bCond, x, ...) if (bCond) { LOG_SEVERITY(x, Info, ##__VA_ARGS__); }

#if defined(DEBUG_LOGGING) && DEBUG_LOGGING
#define LOG_D(x, ...) LOG_SEVERITY(x, Debug, ##__VA_ARGS__)
#define LOGIF_D(bCond, x, ...) if (bCond) { LOG_SEVERITY(x, Debug, ##__VA_ARGS__);  }
#else
#define LOG_D(x, ...)
#define LOGIF_D(bCond, x, ...)
#endif

namespace LE
{
enum class LogSeverity
{
	Debug,
	Info,
	Warning,
	Error
};

extern LogSeverity g_MinLogSeverity;
extern std::function<void(std::string logStr)> g_onLogStr;

void Log(LogSeverity severity, const char* szText, ...);

std::string_view ParseSeverity(LogSeverity severity);
LogSeverity ParseSeverity(std::string_view serialised);
} // namespace LE
