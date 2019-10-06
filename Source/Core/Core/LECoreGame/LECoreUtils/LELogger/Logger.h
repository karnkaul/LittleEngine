#pragma once
#include <functional>
#include <string>

#define LOG_SEVERITY(msg, severity, ...) LELog(LELogSeverity::severity, msg, ##__VA_ARGS__)
#define LOG_E(x, ...) LOG_SEVERITY(x, Error, ##__VA_ARGS__)
#define LOG_W(x, ...) LOG_SEVERITY(x, Warning, ##__VA_ARGS__)
#define LOG_I(x, ...) LOG_SEVERITY(x, Info, ##__VA_ARGS__)
#define LOGIF_E(bCond, x, ...) if (bCond) { LOG_SEVERITY(x, Error, ##__VA_ARGS__); }
#define LOGIF_W(bCond, x, ...) if (bCond) { LOG_SEVERITY(x, Warning, ##__VA_ARGS__); }
#define LOGIF_I(bCond, x, ...) if (bCond) { LOG_SEVERITY(x, Info, ##__VA_ARGS__); }

enum class LELogSeverity
{
	Debug,
	Info,
	Warning,
	Error
};

extern LELogSeverity LE_g_MinLogSeverity;
extern std::function<void(std::string logStr)> LE_g_onLogStr;

void LELog(LELogSeverity severity, const char* szText, ...);

std::string_view LEParseLogSeverity(LELogSeverity severity);
LELogSeverity LEParseLogSeverity(std::string_view serialised);
