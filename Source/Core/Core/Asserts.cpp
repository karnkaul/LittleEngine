#include <cassert>
#include <iostream>
#include <string>
#include <unordered_set>
#include "Asserts.h"
#include "OS.h"
#include "SysDialog.h"
#include "Logger.h"

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
		LOG_E("Assertion failed: %s", message);
		if (OS::IsDebuggerAttached())
		{
			OS::DebugBreak();
		}
		else
		{
#if defined(DEBUGGING)
			OS::DebugBreak();
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
