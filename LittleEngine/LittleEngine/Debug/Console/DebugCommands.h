#pragma once
#include "Core/CoreTypes.h"
#if ENABLED(CONSOLE)
namespace LittleEngine
{
namespace Debug
{
struct LogLine;

namespace Commands
{
struct AutoCompleteResults
{
	Vec<String> queries;
	Vec<String> params;
	bool bCustomParam = false;
};

void Init();
Vec<LogLine> Execute(const String& query);
AutoCompleteResults AutoComplete(const String& incompleteQuery);
} // namespace Commands
} // namespace Debug
} // namespace LittleEngine
#endif
