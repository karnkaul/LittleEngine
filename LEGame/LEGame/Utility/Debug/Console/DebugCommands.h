#pragma once
#include "Core/CoreTypes.h"
#include "LogLine.h"

#if ENABLED(CONSOLE)
namespace LittleEngine
{
class GameManager;
class LEContext;

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

// Simple command with no params, or that parses its own params dynamically
class Command
{
public:
	String m_name;
	// Set to true to include space when autocompleting this Command
	bool m_bTakesCustomParam = false;

	virtual ~Command() = default;

	Vec<LogLine> Execute(String params);

	virtual Vec<String> AutoCompleteParams(const String& /*query*/);

protected:
	Vec<LogLine> m_executeResult;

	Command(const char* szName);

	virtual void FillExecuteResult(String params) = 0;
};

void Init(LEContext& context);
void Cleanup();
void AddCommand(UPtr<Command> uCommand);
Vec<LogLine> Execute(const String& query);
AutoCompleteResults AutoComplete(const String& incompleteQuery);
} // namespace Commands
} // namespace Debug
} // namespace LittleEngine
#endif
