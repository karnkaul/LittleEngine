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
	std::vector<std::string> queries;
	std::vector<std::string> params;
	bool bCustomParam = false;
};

// Simple command with no params, or that parses its own params dynamically
class Command
{
public:
	std::string m_name;
	// Set to true to include space when autocompleting this Command
	bool m_bTakesCustomParam = false;

	virtual ~Command() = default;

	std::vector<LogLine> Execute(std::string params);

	virtual std::vector<std::string> AutoCompleteParams(std::string_view /*query*/);

protected:
	std::vector<LogLine> m_executeResult;

	Command(std::string_view szName);

	virtual void FillExecuteResult(std::string params) = 0;
};

void Init(LEContext& context);
void Cleanup();
void AddCommand(UPtr<Command> uCommand);
std::vector<LogLine> Execute(std::string_view query);
AutoCompleteResults AutoComplete(std::string_view incompleteQuery);
} // namespace Commands
} // namespace Debug
} // namespace LittleEngine
#endif
