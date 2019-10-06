#include <cassert>
#include <iostream>
#include <string>
#include <unordered_set>
#if _WIN64
#include <Windows.h>
#elif __linux__
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#include "Asserts.h"
#include "LESysDialog/SysDialog.h"

namespace Core
{
namespace
{
enum class ResponseType : u8
{
	Assert = 0,
	Ignore,
	Disable,
};

std::unordered_set<std::string> disabledIDs;

bool IsDebuggerAttached()
{
	bool ret = false;
#if _WIN64
	ret = IsDebuggerPresent();
#elif __linux__
	char buf[4096];

	const auto status_fd = ::open("/proc/self/status", O_RDONLY);
	if (status_fd == -1)
		return false;

	const auto num_read = ::read(status_fd, buf, sizeof(buf) - 1);
	if (num_read <= 0)
		return false;

	buf[num_read] = '\0';
	constexpr char tracerPidString[] = "TracerPid:";
	const auto tracer_pid_ptr = ::strstr(buf, tracerPidString);
	if (!tracer_pid_ptr)
		return false;

	for (const char* characterPtr = tracer_pid_ptr + sizeof(tracerPidString) - 1; characterPtr <= buf + num_read; ++characterPtr)
	{
		if (::isspace(*characterPtr))
		{
			continue;
		}
		else
		{
			ret = ::isdigit(*characterPtr) != 0 && *characterPtr != '0';
		}
	}
#endif
	return ret;
}

void DebugBreak()
{
#if defined(CXX_MS_CRT)
	__debugbreak();
#elif defined(CXX_LIBC) || defined(CXX_LIBSTDC)
#ifdef SIGTRAP
	raise(SIGTRAP);
#else
	raise(SIGILL);
#endif
#endif
}
} // namespace

void AssertWithMsg(bool expr, const char* message, const char* fileName, long lineNumber)
{
	if (expr)
	{
		return;
	}

	std::string assertID = std::string(message) + "_" + fileName + "_" + std::to_string(lineNumber);
	if (disabledIDs.find(assertID) != disabledIDs.end())
	{
		return;
	}

	ResponseType response = ResponseType::Assert;

	DialogueData data;
	data.resp0 = {"Assert", [&]() { response = ResponseType::Assert; }};
	data.resp1 = {"Ignore", [&]() { response = ResponseType::Ignore; }};
	data.resp2 = {"Disable", [&]() { response = ResponseType::Disable; }};

	data.title = "Assertion Failed!";
	data.content = message;
	data.footer = "[" + std::string(fileName) + ": " + std::to_string(lineNumber) + "]";

	CreateSystemDialogue(std::move(data));
	switch (response)
	{
	case ResponseType::Assert:
	{
		std::cerr << "Assertion failed: " << message;
		if (IsDebuggerAttached())
		{
#if _MSC_VER
			OutputDebugStringA(message);
#endif
			DebugBreak();
		}
		else
		{
#if defined(DEBUGGING)
			DebugBreak();
#else
			assert(false && message);
#endif
		}
		break;
	}
	case ResponseType::Disable:
	{
		disabledIDs.emplace(assertID);
		break;
	}
	default:
	{
		break;
	}
	}
}
} // namespace Core
