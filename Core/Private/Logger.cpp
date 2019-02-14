#include "stdafx.h"
#include <iostream>
#if _WIN32
#include "Windows.h"
#endif
#include "Logger.h"

namespace Core
{
namespace
{
String Cast(LogSeverity severity, size_t numChars = 1)
{
	switch (severity)
	{
	case LogSeverity::Debug:
		return String("Debug").substr(0, numChars);
	case LogSeverity::Info:
		return String("Info").substr(0, numChars);
	case LogSeverity::Warning:
		return String("Warning").substr(0, numChars);
	case LogSeverity::Error:
		return String("Error").substr(0, numChars);
	case LogSeverity::HOT:
		return String("HOT").substr(0, numChars);
	}
}
} // namespace

LogSeverity g_MinLogSeverity = LogSeverity::Info;
Function(void(const String&)) g_OnLogCallback = nullptr;

void LogOutput(StringStream& stream, LogSeverity severity)
{
	LogOutput(stream.str(), severity);
}

void LogOutput(const String& str, LogSeverity severity)
{
	if (static_cast<s32>(severity) < static_cast<s32>(g_MinLogSeverity))
		return;

	String output = "[" + Cast(severity) + "] " + str + "\n";
	std::cout << output;

	if (g_OnLogCallback)
		g_OnLogCallback(output);
#if _WIN32
	OutputDebugStringA(output.c_str());
#endif
}
} // namespace Core
