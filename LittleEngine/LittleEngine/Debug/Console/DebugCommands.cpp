#include "stdafx.h"
#include "DebugCommands.h"
#if ENABLED(CONSOLE)
#include "Utils.h"
#include "DebugConsole.h"
#include "RenderStatsRenderer.h"
#include "LittleEngine/Debug/DebugProfiler.h"
#include "LittleEngine/Engine/EngineService.h"
#include "LittleEngine/Game/World/World.h"
#include "LittleEngine/Game/World/WorldStateMachine.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/Physics/CollisionManager.h"

namespace LittleEngine
{
namespace Debug
{
namespace Console
{
extern void Quit();
}

namespace Commands
{
namespace
{
Vector<LogLine> GetAllCommands();
}

#pragma region Commands
class Command
{
public:
	String name;

	virtual ~Command()
	{
	}

	Vector<LogLine> Execute(const String& params)
	{
		executeResult.emplace_back(name + " " + params, g_liveHistoryColour);
		FillExecuteResult(params);
		return std::move(executeResult);
	}

	virtual Vector<String> AutoCompleteParams(const String&)
	{
		return Vector<String>();
	}

protected:
	Vector<LogLine> executeResult;

	Command(const String& name) : name(std::move(name))
	{
	}

	virtual void FillExecuteResult(const String& params) = 0;
};

class ParameterisedCommand : public Command
{
public:
	virtual void FillExecuteResult(const String& params) override final
	{
		if (params.empty())
		{
			executeResult.emplace_back(OnEmptyParams());
		}

		else
		{
			auto search = paramCallbackMap.find(params);
			if (search != paramCallbackMap.end())
			{
				search->second(executeResult);
			}
			else
			{
				executeResult.emplace_back("Unknown parameters: " + params, g_logWarningColour);
			}
		}
	}
	virtual Vector<String> AutoCompleteParams(const String& incompleteParams) override final
	{
		Vector<String> params;
		/*if (!incompleteParams.empty())*/ {
			for (const auto& p : paramCallbackMap)
			{
				if (incompleteParams.empty() ||
					(p.first.find(incompleteParams) != String::npos && incompleteParams[0] == p.first[0]))
				{
					params.emplace_back(p.first);
				}
			}
		}
		return params;
	}

protected:
	Map<String, void (*)(Vector<LogLine>&)> paramCallbackMap;

	ParameterisedCommand(const String& name) : Command(name)
	{
	}

	virtual LogLine OnEmptyParams() = 0;
};

class HelpCommand : public Command
{
public:
	HelpCommand() : Command("help")
	{
	}

	virtual void FillExecuteResult(const String&) override
	{
		executeResult = GetAllCommands();
	}
};

class ShowCommand : public ParameterisedCommand
{
public:
	ShowCommand() : ParameterisedCommand("show")
	{
		paramCallbackMap.emplace("colliders", [](Vector<LogLine>& executeResult) {
#if DEBUGGING
			Services::Game()->Physics()->ToggleDebugShapes(true);
			executeResult.emplace_back("Turned on Debug Colliders", g_logTextColour);
#else
		executeResult.emplace_back("Collider Debug shapes not available", g_logTextColour);
#endif
		});

		paramCallbackMap.emplace("profiler", [](Vector<LogLine>& executeResult) {
#if ENABLED(PROFILER)
			Profiler::Toggle(true);
			executeResult.emplace_back("Turned on Profiler", g_logTextColour);
#else
		executeResult.emplace_back("Profiler not enabled", g_logTextColour);
#endif
		});
		paramCallbackMap.emplace("renderStats", [](Vector<LogLine>& executeResult) {
			RenderStatsRenderer::s_bConsoleRenderStatsEnabled = true;
			executeResult.emplace_back("Turned on Render Stats", g_logTextColour);
		});
	}

protected:
	virtual LogLine OnEmptyParams() override
	{
		return LogLine("show what?", g_logTextColour);
	}
};

class HideCommand : public ParameterisedCommand
{
public:
	HideCommand() : ParameterisedCommand("hide")
	{
		paramCallbackMap.emplace("colliders", [](Vector<LogLine>& executeResult) {
#if DEBUGGING
			Services::Game()->Physics()->ToggleDebugShapes(false);
			executeResult.emplace_back("Turned off Debug Colliders", g_logTextColour);
#else
			executeResult.emplace_back("Collider Debug shapes not available", g_logTextColour);
#endif
		});
		paramCallbackMap.emplace("profiler", [](Vector<LogLine>& executeResult) {
#if ENABLED(PROFILER)
			Profiler::Toggle(false);
			executeResult.emplace_back("Turned off Profiler", g_logTextColour);
#else
			executeResult.emplace_back("Profiler not enabled", g_logTextColour);
#endif
		});
		paramCallbackMap.emplace("renderStats", [](Vector<LogLine>& executeResult) {
			RenderStatsRenderer::s_bConsoleRenderStatsEnabled = false;
			executeResult.emplace_back("Turned off RenderStats", g_logTextColour);
		});
	}

protected:
	virtual LogLine OnEmptyParams() override
	{
		return LogLine("hide what?", g_logTextColour);
	}
};

class QuitCommand : public Command
{
public:
	QuitCommand() : Command("quit")
	{
	}

	virtual void FillExecuteResult(const String&) override
	{
		if (WorldStateMachine::s_bReady)
		{
			executeResult.emplace_back("Quitting instantly", g_logWarningColour);
			Console::Quit();
		}
		else
		{
			executeResult.emplace_back(
				"Cannot quit until WorldStateMachine's load jobs are complete!", g_logWarningColour);
		}
	}
};

class LoadWorldCommand : public Command
{
public:
	LoadWorldCommand() : Command("loadworld")
	{
	}

	virtual void FillExecuteResult(const String& params) override
	{
		if (params.empty())
		{
			executeResult.emplace_back("Syntax: loadworld <id>", g_logTextColour);
			return;
		}

		s32 worldID = Strings::ToS32(params);
		bool success = false;
		if (worldID >= 0)
		{
			success = Services::Engine()->Worlds()->LoadState(worldID);
		}
		if (success)
		{
			executeResult.emplace_back("Loading World ID: " + Strings::ToString(worldID), g_logTextColour);
		}
		else
		{
			executeResult.emplace_back("Invalid WorldID " + params, g_logWarningColour);
		}
	}
};
#pragma endregion
#pragma region Local Namespace Impl
namespace
{
Map<String, UPtr<Command>> commands;

Vector<LogLine> GetAllCommands()
{
	Vector<LogLine> result;
	result.emplace_back("Registered commands:", g_logTextColour);
	for (auto& command : commands)
	{
		result.emplace_back("\t" + command.second->name, g_logTextColour);
	}
	return result;
}

String StripPaddedSpaces(const String& input)
{
	size_t firstNonSpaceIdx = 0;
	while (input[firstNonSpaceIdx] == ' ' && firstNonSpaceIdx < input.length())
	{
		++firstNonSpaceIdx;
	}
	size_t lastNonSpaceIdx = input.length() - 1;
	while (input[lastNonSpaceIdx] == ' ' && lastNonSpaceIdx > firstNonSpaceIdx)
	{
		--lastNonSpaceIdx;
	}
	return input.substr(firstNonSpaceIdx, lastNonSpaceIdx - firstNonSpaceIdx + 1);
}

void SplitQuery(String& outQuery, String& outCommand, String& outParams)
{
	outQuery = StripPaddedSpaces(outQuery);
	outParams = "";
	if (outQuery.empty())
	{
		outCommand = "";
		return;
	}

	size_t delimiterIdx = 0;
	while (outQuery[delimiterIdx] != ' ' && delimiterIdx != outQuery.length())
	{
		++delimiterIdx;
	}
	outCommand = outQuery.substr(0, delimiterIdx);
	if (delimiterIdx + 1 < outQuery.length())
	{
		outParams = outQuery.substr(delimiterIdx + 1, outQuery.length() - delimiterIdx);
	}
}

Vector<LogLine> ExecuteQuery(const String& command, const String& params)
{
	Vector<LogLine> ret;
	auto search = commands.find(command);
	if (search != commands.end())
	{
		ret = search->second->Execute(params);
	}
	return ret;
}

Vector<Command*> FindCommands(const String& incompleteCommand)
{
	Vector<Command*> results;
	for (auto& command : commands)
	{
		String name = command.second->name;
		if (name.find(incompleteCommand) != String::npos)
		{
			results.push_back(command.second.get());
		}
	}
	return results;
}
} // namespace
#pragma endregion

#pragma region Implementation
void Init()
{
	commands.emplace("help", MakeUnique<HelpCommand>());
	commands.emplace("show", MakeUnique<ShowCommand>());
	commands.emplace("hide", MakeUnique<HideCommand>());
	commands.emplace("quit", MakeUnique<QuitCommand>());
	commands.emplace("loadworld", MakeUnique<LoadWorldCommand>());
}

Vector<LogLine> Execute(const String& query)
{
	String cleanedQuery(query);
	String command;
	String params;
	SplitQuery(cleanedQuery, command, params);
	Vector<LogLine> ret;
	if (!cleanedQuery.empty())
	{
		ret = Commands::ExecuteQuery(command, params);
		if (ret.empty())
		{
			ret.emplace_back("Unrecognised command: " + command, g_logWarningColour);
		}
	}
	return ret;
}

AutoCompleteResults AutoComplete(const String& incompleteQuery)
{
	String cleanedQuery(incompleteQuery);
	String incompleteCommand;
	String incompleteParams;
	SplitQuery(cleanedQuery, incompleteCommand, incompleteParams);

	Vector<Command*> matchedCommands = FindCommands(incompleteCommand);
	AutoCompleteResults results;
	if (!matchedCommands.empty())
	{
		// If exact match, build auto-compeleted params for the command
		if (matchedCommands.size() == 1 && matchedCommands[0]->name == incompleteCommand)
		{
			Vector<String> matchedParams = matchedCommands[0]->AutoCompleteParams(incompleteParams);
			for (const auto& p : matchedParams)
			{
				results.params.emplace_back(std::move(p));
			}
		}

		// If no params, return matchedCommands
		if (incompleteParams.empty())
		{
			for (auto command : matchedCommands)
			{
				results.queries.emplace_back(command->name);
			}
		}
		else
		{
			for (auto command : matchedCommands)
			{
				Vector<String> matchedParams = command->AutoCompleteParams(incompleteParams);
				for (const auto& p : matchedParams)
				{
					String suffix = p.empty() ? "" : " " + p;
					results.queries.push_back(command->name + suffix);
				}
			}
		}
	}
	return results;
}
} // namespace Commands
#pragma endregion
} // namespace Debug
} // namespace LittleEngine
#endif