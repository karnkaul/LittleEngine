#include "stdafx.h"
#include <functional>
#include "DebugCommands.h"
#if ENABLED(CONSOLE)
#include "Utils.h"
#include "DebugConsole.h"
#include "RenderStatsRenderer.h"
#include "Logger.h"
#include "LogLine.h"
#include "SFMLAPI/System/SFAssets.h"
#include "LittleEngine/Debug/DebugProfiler.h"
#include "LittleEngine/Engine/EngineService.h"
#include "LittleEngine/Game/GameSettings.h"
#include "LittleEngine/Game/GameManager.h"
#include "LittleEngine/Game/World/World.h"
#include "LittleEngine/Game/World/WorldStateMachine.h"
#include "LittleEngine/GFX/GFX.h"
#include "LittleEngine/Physics/CollisionManager.h"
#include "LittleEngine/Repository/EngineRepository.h"
#include "LittleEngine/Services/Services.h"

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
Vec<LogLine> GetAllCommands();
}

#pragma region Commands
// Simple command with no params, or that parses its own params dynamically
class Command
{
public:
	String name;
	// Set to true to include space when autocompleting this Command
	bool bTakesCustomParam = false;

	virtual ~Command() = default;

	Vec<LogLine> Execute(String params)
	{
		String suffix = params.empty() ? "" : " " + params;
		executeResult.emplace_back(name + suffix, g_liveHistoryColour);
		FillExecuteResult(std::move(params));
		return std::move(executeResult);
	}

	virtual Vec<String> AutoCompleteParams(const String& /*query*/)
	{
		return Vec<String>();
	}

protected:
	Vec<LogLine> executeResult;

	Command(const char* name) : name(name)
	{
	}

	virtual void FillExecuteResult(String params) = 0;
};

// Commands that take a fixed number and constant values of parameters as options
class ParameterisedCommand : public Command
{
public:
	void FillExecuteResult(String params) final
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

	Vec<String> AutoCompleteParams(const String& incompleteParams) final
	{
		Vec<String> params;
		for (const auto& p : paramCallbackMap)
		{
			if (incompleteParams.empty() ||
				(p.first.find(incompleteParams) != String::npos && incompleteParams[0] == p.first[0]))
			{
				params.emplace_back(p.first);
			}
		}
		return params;
	}

protected:
	Map<String, std::function<void(Vec<LogLine>&)>> paramCallbackMap;

	ParameterisedCommand(const char* name) : Command(name)
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

	void FillExecuteResult(String /*params*/) override
	{
		executeResult = GetAllCommands();
	}
};

class ShowCommand : public ParameterisedCommand
{
public:
	ShowCommand() : ParameterisedCommand("show")
	{
		paramCallbackMap.emplace("colliders", [](Vec<LogLine>& executeResult) {
#if DEBUGGING
			Services::Game()->Physics()->ToggleDebugShapes(true);
			executeResult.emplace_back("Turned on Debug Colliders", g_logTextColour);
#else
		executeResult.emplace_back("Collider Debug shapes not available", g_logTextColour);
#endif
		});

		paramCallbackMap.emplace("profiler", [](Vec<LogLine>& executeResult) {
#if ENABLED(PROFILER)
			Profiler::Toggle(true);
			executeResult.emplace_back("Turned on Profiler", g_logTextColour);
#else
		executeResult.emplace_back("Profiler not enabled", g_logTextColour);
#endif
		});
		paramCallbackMap.emplace("renderStats", [](Vec<LogLine>& executeResult) {
			RenderStatsRenderer::s_bConsoleRenderStatsEnabled = true;
			executeResult.emplace_back("Turned on Render Stats", g_logTextColour);
		});
	}

protected:
	LogLine OnEmptyParams() override
	{
		return LogLine("show what?", g_logTextColour);
	}
};

class HideCommand : public ParameterisedCommand
{
public:
	HideCommand() : ParameterisedCommand("hide")
	{
		paramCallbackMap.emplace("colliders", [](Vec<LogLine>& executeResult) {
#if DEBUGGING
			Services::Game()->Physics()->ToggleDebugShapes(false);
			executeResult.emplace_back("Turned off Debug Colliders", g_logTextColour);
#else
			executeResult.emplace_back("Collider Debug shapes not available", g_logTextColour);
#endif
		});
		paramCallbackMap.emplace("profiler", [](Vec<LogLine>& executeResult) {
#if ENABLED(PROFILER)
			Profiler::Toggle(false);
			executeResult.emplace_back("Turned off Profiler", g_logTextColour);
#else
			executeResult.emplace_back("Profiler not enabled", g_logTextColour);
#endif
		});
		paramCallbackMap.emplace("renderStats", [](Vec<LogLine>& executeResult) {
			RenderStatsRenderer::s_bConsoleRenderStatsEnabled = false;
			executeResult.emplace_back("Turned off RenderStats", g_logTextColour);
		});
	}

protected:
	LogLine OnEmptyParams() override
	{
		return LogLine("hide what?", g_logTextColour);
	}
};

class ResolutionCommand : public ParameterisedCommand
{
public:
	ResolutionCommand() : ParameterisedCommand("resolution")
	{
		const Map<u32, SFWindowSize>& windowSizes = GFX::GetValidWindowSizes();
		for (const auto& kvp : windowSizes)
		{
			const auto& windowSize = kvp.second;
			String windowSizeText = Strings::ToString(windowSize.height) + "p";
			paramCallbackMap.emplace(windowSizeText, 
			[w = windowSize.width, h = windowSize.height ](Vec<LogLine>& executeResult) 
			{
				Services::Engine()->TrySetWindowSize(h);
				String sizeText =
					Strings::ToString(w) + "x" + Strings::ToString(h);
				executeResult.emplace_back("Set Resolution to: " + sizeText, g_logTextColour);
			});
		}
	}

protected:
	LogLine OnEmptyParams() override
	{
		return LogLine("set resolution to what height?", g_logTextColour);
	}
};

class LogLevelCommand : public ParameterisedCommand
{
public:
	LogLevelCommand() : ParameterisedCommand("loglevel")
	{
		paramCallbackMap.emplace("HOT", [](Vec<LogLine>& executeResult) {
			Core::g_MinLogSeverity = Core::LogSeverity::HOT;
			executeResult.emplace_back("Set LogLevel to [HOT]", g_logTextColour);
		});
		paramCallbackMap.emplace("Error", [](Vec<LogLine>& executeResult) {
			Core::g_MinLogSeverity = Core::LogSeverity::Error;
			executeResult.emplace_back("Set LogLevel to [Error]", g_logTextColour);
		});
		paramCallbackMap.emplace("Warning", [](Vec<LogLine>& executeResult) {
			Core::g_MinLogSeverity = Core::LogSeverity::Warning;
			executeResult.emplace_back("Set LogLevel to [Warning]", g_logTextColour);
		});
		paramCallbackMap.emplace("Info", [](Vec<LogLine>& executeResult) {
			Core::g_MinLogSeverity = Core::LogSeverity::Info;
			executeResult.emplace_back("Set LogLevel to [Info]", g_logTextColour);
		});
#if DEBUG_LOGGING
		paramCallbackMap.emplace("Debug", [](Vec<LogLine>& executeResult) {
			Core::g_MinLogSeverity = Core::LogSeverity::Debug;
			executeResult.emplace_back("Set LogLevel to [Debug]", g_logTextColour);
		});
#endif
	}

protected:
	LogLine OnEmptyParams() override
	{
		return LogLine("set g_MinLogSeverity to what?", g_logTextColour);
	}
};

class QuitCommand : public Command
{
public:
	QuitCommand() : Command("quit")
	{
	}

	void FillExecuteResult(String /*params*/) override
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
		bTakesCustomParam = true;
	}

	void FillExecuteResult(String params) override
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

//class BorderlessCommand : public ParameterisedCommand
//{
//public:
//	BorderlessCommand() : ParameterisedCommand("borderless")
//	{
//		paramCallbackMap.emplace("true", 
//		[](Vec<LogLine>& executeResult) 
//		{
//			if (!GameSettings::Instance()->IsBorderless())
//			{
//				GameSettings::Instance()->SetBorderless(true);
//				Services::Engine()->RecreateWindow();
//				executeResult.emplace_back("Borderless activated", g_logTextColour);
//			}
//			else
//			{
//				executeResult.emplace_back("Window is already borderless", g_logTextColour);
//			}
//		}
//		);
//		paramCallbackMap.emplace("false", [](Vec<LogLine>& executeResult) {
//			if (GameSettings::Instance()->IsBorderless())
//			{
//				GameSettings::Instance()->SetBorderless(false);
//				Services::Engine()->RecreateWindow();
//				executeResult.emplace_back("Borderless deactivated", g_logTextColour);
//			}
//			else
//			{
//				executeResult.emplace_back("Window already has a border", g_logTextColour);
//			}
//		});
//	}
//
//protected:
//	LogLine OnEmptyParams() override
//	{
//		return LogLine("Enter \"true\" or \"false\"", g_logTextColour);
//	}
//};
#pragma endregion
#pragma region Local Namespace Impl
namespace
{
Map<String, UPtr<Command>> commands;

Vec<LogLine> GetAllCommands()
{
	Vec<LogLine> result;
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

Vec<LogLine> ExecuteQuery(const String& command, String params)
{
	Vec<LogLine> ret;
	auto search = commands.find(command);
	if (search != commands.end())
	{
		ret = search->second->Execute(std::move(params));
	}
	return ret;
}

Vec<Command*> FindCommands(const String& incompleteCommand, bool bFirstCharMustMatch)
{
	Vec<Command*> results;
	for (auto& command : commands)
	{
		String name = command.second->name;
		if (name.find(incompleteCommand) != String::npos)
		{
			if (!bFirstCharMustMatch || command.second->name[0] == incompleteCommand[0])
			{
				results.push_back(command.second.get());
			}
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
	commands.emplace("resolution", MakeUnique<ResolutionCommand>());
	commands.emplace("loglevel", MakeUnique<LogLevelCommand>());
	//commands.emplace("borderless", MakeUnique<BorderlessCommand>());
}

Vec<LogLine> Execute(const String& query)
{
	String cleanedQuery(query);
	String command;
	String params;
	SplitQuery(cleanedQuery, command, params);
	Vec<LogLine> ret;
	if (!cleanedQuery.empty())
	{
		ret = Commands::ExecuteQuery(command, std::move(params));
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

	Vec<Command*> matchedCommands = FindCommands(incompleteCommand, true);
	AutoCompleteResults results;
	if (!matchedCommands.empty())
	{
		// If exact match, build auto-compeleted params for the command
		if (matchedCommands.size() == 1)
		{
			Vec<String> matchedParams = matchedCommands[0]->AutoCompleteParams(incompleteParams);
			for (auto& p : matchedParams)
			{
				results.params.emplace_back(std::move(p));
			}
			results.bCustomParam = matchedCommands[0]->bTakesCustomParam;
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
				Vec<String> matchedParams = command->AutoCompleteParams(incompleteParams);
				for (const auto& p : matchedParams)
				{
					String suffix = p.empty() ? "" : " " + p;
					results.queries.emplace_back(command->name + suffix);
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
