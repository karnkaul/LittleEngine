#include "DebugCommands.h"
#if ENABLED(CONSOLE)
#include "Core/Game/LECoreUtils/Utils.h"
#include "SFMLAPI/System/Assets.h"
#include "Engine/GFX.h"
#include "Engine/Debug/Tweakable.h"
#include "Engine/Context/LEContext.h"
#include "Engine/Physics/LEPhysics.h"
#include "Engine/Rendering/LERenderer.h"
#include "Engine/Repository/LERepository.h"
#include "DebugConsole.h"
#include "LogLine.h"
#include "Model/GameSettings.h"
#include "Model/GameManager.h"
#include "Model/World/World.h"
#include "Model/World/WorldStateMachine.h"
#include "Framework/Utility/Debug/DebugProfiler.h"
#include "Framework/Utility/Debug/RenderStatsRenderer.h"

namespace LittleEngine::Debug
{
namespace Console
{
extern void Quit();
}

namespace Commands
{
namespace
{
LEContext* pContext = nullptr;
std::vector<LogLine> AllCommands();
} // namespace

#pragma region Commands
Command::Command(std::string_view name) : m_name(name) {}

std::vector<LogLine> Command::Execute(std::string params)
{
	std::string suffix = params.empty() ? "" : " " + params;
	m_executeResult.emplace_back(m_name + suffix, g_liveHistoryColour);
	FillExecuteResult(std::move(params));
	return (m_executeResult);
}

std::vector<std::string> Command::AutoCompleteParams(std::string_view /*incomplete*/)
{
	return std::vector<std::string>();
}

// Commands that take a fixed number and constant values of parameters as options
class ParameterisedCommand : public Command
{
public:
	void FillExecuteResult(std::string params) final
	{
		if (params.empty())
		{
			m_executeResult.emplace_back(OnEmptyParams());
		}

		else
		{
			auto search = m_paramCallbackMap.find(params);
			if (search != m_paramCallbackMap.end())
			{
				search->second(m_executeResult);
			}
			else
			{
				m_executeResult.emplace_back("Unknown parameters: " + params, g_logWarningColour);
			}
		}
	}

	std::vector<std::string> AutoCompleteParams(std::string_view incompleteParams) final
	{
		std::vector<std::string> params;
		for (const auto& p : m_paramCallbackMap)
		{
			if (incompleteParams.empty() || (p.first.find(incompleteParams) != std::string::npos && incompleteParams[0] == p.first[0]))
			{
				params.emplace_back(p.first);
			}
		}
		return params;
	}

protected:
	std::map<std::string, std::function<void(std::vector<LogLine>&)>> m_paramCallbackMap;

	ParameterisedCommand(std::string_view name) : Command(name) {}

	virtual LogLine OnEmptyParams()
	{
		return LogLine("Syntax: " + m_name + "<params>", g_logTextColour);
	}
};

class BooleanCommand : public ParameterisedCommand
{
public:
	BooleanCommand(std::string_view name) : ParameterisedCommand(name)
	{
		m_paramCallbackMap.emplace("true", [&](std::vector<LogLine>& executeResult) { OnTrue(executeResult); });
		m_paramCallbackMap.emplace("false", [&](std::vector<LogLine>& executeResult) { OnFalse(executeResult); });
	}

protected:
	virtual void OnTrue(std::vector<LogLine>& executeResult) = 0;
	virtual void OnFalse(std::vector<LogLine>& executeResult) = 0;
};

class Help : public Command
{
public:
	Help() : Command("help") {}

	void FillExecuteResult(std::string /*params*/) override
	{
		m_executeResult = AllCommands();
	}
};

class LoadWorld : public Command
{
public:
	LoadWorld() : Command("loadWorld")
	{
		m_bTakesCustomParam = true;
	}

	void FillExecuteResult(std::string params) override
	{
		if (params.empty())
		{
			m_executeResult.emplace_back("Syntax: " + m_name + " <id/reload>", g_logTextColour);
			return;
		}
		Strings::ToLower(params);
		s32 currentWorldID = g_pGameManager->ActiveWorldID();
		if (params == "reload")
		{
			m_executeResult.emplace_back("Reloading World " + Strings::ToString(currentWorldID), g_logTextColour);
			g_pGameManager->ReloadWorld();
			return;
		}
		s32 worldID = Strings::ToS32(params);
		Assert(g_pGameManager, "Game Manager is null!");
		bool success = false;
		if (worldID >= 0)
		{
			if (worldID == currentWorldID)
			{
				m_executeResult.emplace_back("Already on World " + Strings::ToString(worldID), g_logTextColour);
				return;
			}
			success = g_pGameManager->LoadWorld(worldID);
		}
		if (success)
		{
			m_executeResult.emplace_back("Loading World " + Strings::ToString(worldID), g_logTextColour);
		}
		else
		{
			m_executeResult.emplace_back("Failed to load WorldID: " + params, g_logWarningColour);
		}
	}
};

class Quit : public Command
{
public:
	Quit() : Command("quit") {}

	void FillExecuteResult(std::string /*params*/) override
	{
		if (WorldStateMachine::s_bRunning)
		{
			m_executeResult.emplace_back("Quitting instantly", g_logWarningColour);
			Console::Quit();
		}
		else
		{
			m_executeResult.emplace_back("Cannot quit while WorldStateMachine is busy!", g_logWarningColour);
		}
	}
};

class LogLevel : public ParameterisedCommand
{
public:
	LogLevel() : ParameterisedCommand("logLevel")
	{
		m_paramCallbackMap.emplace("Error", [](std::vector<LogLine>& executeResult) {
			LE::g_MinLogSeverity = LE::LogSeverity::Error;
			executeResult.emplace_back("Set LogLevel to [Error]", g_logTextColour);
		});
		m_paramCallbackMap.emplace("Warning", [](std::vector<LogLine>& executeResult) {
			LE::g_MinLogSeverity = LE::LogSeverity::Warning;
			executeResult.emplace_back("Set LogLevel to [Warning]", g_logTextColour);
		});
		m_paramCallbackMap.emplace("Info", [](std::vector<LogLine>& executeResult) {
			LE::g_MinLogSeverity = LE::LogSeverity::Info;
			executeResult.emplace_back("Set LogLevel to [Info]", g_logTextColour);
		});
#if ENABLED(DEBUG_LOGGING)
		m_paramCallbackMap.emplace("Debug", [](std::vector<LogLine>& executeResult) {
			LE::g_MinLogSeverity = LE::LogSeverity::Debug;
			executeResult.emplace_back("Set LogLevel to [Debug]", g_logTextColour);
		});
#endif
	}
};

class Resolution : public ParameterisedCommand
{
public:
	Resolution() : ParameterisedCommand("resolution")
	{
		const std::map<u32, ViewportSize>& windowSizes = g_pGFX->ValidViewportSizes();
		for (const auto& kvp : windowSizes)
		{
			const auto& windowSize = kvp.second;
			std::string windowSizeText = Strings::ToString(windowSize.height) + "p";
			m_paramCallbackMap.emplace(windowSizeText, [w = windowSize.width, h = windowSize.height](std::vector<LogLine>& executeResult) {
				pContext->TrySetViewportSize(h);
				std::string sizeText = Strings::ToString(w) + "x" + Strings::ToString(h);
				executeResult.emplace_back("Set Resolution to: " + sizeText, g_logTextColour);
			});
		}
	}
};

#if ENABLED(TWEAKABLES)
class Set : public Command
{
public:
	Set() : Command("set")
	{
		m_bTakesCustomParam = true;
	}

	void FillExecuteResult(std::string params) override
	{
		std::vector<std::string> tokens = Strings::Tokenise(params, ' ', {});
		if (tokens.empty())
		{
			m_executeResult.emplace_back("Syntax: " + m_name + " <id> <value>", g_logTextColour);
			return;
		}

		std::string id;
		if (!tokens.empty())
		{
			id = std::move(tokens[0]);
		}
		auto& tweakables = TweakManager::Instance()->m_tweakables;
		auto iter = tweakables.find(id);
		if (iter == tweakables.end())
		{
			m_executeResult.emplace_back("Unknown Tweakable: " + id, g_logWarningColour);
			return;
		}
		if (tokens.size() < 2)
		{
			m_executeResult.emplace_back("Syntax: " + m_name + " " + id + " <value>", g_logTextColour);
			return;
		}

		Tweakable* pTweakable = iter->second;
		if (tokens[1] == "?")
		{
			m_executeResult.emplace_back("\t" + pTweakable->m_id + " = " + pTweakable->m_value, g_logTextColour);
			return;
		}

		pTweakable->Set(std::move(tokens[1]));
		m_executeResult.emplace_back("\t" + pTweakable->m_id + " = " + pTweakable->m_value, g_logTextColour);
	}

	std::vector<std::string> AutoCompleteParams(std::string_view incompleteParams) override
	{
		std::vector<std::string> matches;
		auto& tweakables = TweakManager::Instance()->m_tweakables;
		for (const auto& kvp : tweakables)
		{
			if (kvp.first.find(incompleteParams) != std::string::npos)
			{
				matches.emplace_back(kvp.first);
			}
		}
		return matches;
	}
};

class Unload : public Command
{
public:
	Unload() : Command("unload")
	{
		m_bTakesCustomParam = true;
	}

	void FillExecuteResult(std::string params) override
	{
		if (params.empty())
		{
			m_executeResult.emplace_back("Syntax: " + m_name + " <assetID>", g_logTextColour);
		}
		else
		{
			if (params == "all")
			{
				g_pRepository->UnloadAll(false);
				m_executeResult.emplace_back("Unloaded all except default font from Repository", g_logTextColour);
			}
			else
			{
				if (g_pRepository->Unload(params))
				{
					m_executeResult.emplace_back("Unloaded " + params + " from Repository", g_logTextColour);
				}
				else
				{
					m_executeResult.emplace_back(params + " not loaded in Repository", g_logTextColour);
				}
			}
		}
	}
};
#endif
#pragma endregion
#pragma region Local Namespace Impl
namespace
{
std::list<UPtr<Command>> commands;

template <typename T>
void Add()
{
	static_assert(IsDerived<Command, T>(), "T must derive from Command");
	UPtr<T> uT = std::make_unique<T>();
	auto iter = std::find_if(commands.begin(), commands.end(), [&uT](const UPtr<Command>& uC) { return uT->m_name < uC->m_name; });
	commands.insert(iter, std::move(uT));
}

std::vector<LogLine> AllCommands()
{
	std::vector<LogLine> result;
	result.emplace_back("Registered commands:", g_logTextColour);
	for (auto& uCommand : commands)
	{
		result.emplace_back("\t" + uCommand->m_name, g_logTextColour);
	}
	return result;
}

std::string StripPaddedSpaces(std::string_view input)
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
	return std::string(input.substr(firstNonSpaceIdx, lastNonSpaceIdx - firstNonSpaceIdx + 1));
}

std::string SplitQuery(std::string_view query, std::string& outCommand, std::string& outParams)
{
	std::string ret = StripPaddedSpaces(query);
	outParams = "";
	if (ret.empty())
	{
		outCommand = "";
		return ret;
	}

	size_t delimiterIdx = 0;
	while (ret[delimiterIdx] != ' ' && delimiterIdx != ret.length())
	{
		++delimiterIdx;
	}
	outCommand = ret.substr(0, delimiterIdx);
	if (delimiterIdx + 1 < ret.length())
	{
		outParams = ret.substr(delimiterIdx + 1, ret.length() - delimiterIdx);
	}
	return ret;
}

std::vector<LogLine> ExecuteQuery(const std::string& command, std::string params)
{
	std::vector<LogLine> ret;
	auto search =
		std::find_if(commands.begin(), commands.end(), [&command](const UPtr<Command>& uCommand) { return uCommand->m_name == command; });
	if (search != commands.end())
	{
		ret = (*search)->Execute(std::move(params));
	}
	return ret;
}

std::vector<Command*> FindCommands(const std::string& incompleteCommand, bool bFirstCharMustMatch)
{
	std::vector<Command*> results;
	for (auto& uCommand : commands)
	{
		std::string name = uCommand->m_name;
		if (name.find(incompleteCommand) != std::string::npos)
		{
			if (!bFirstCharMustMatch || uCommand->m_name[0] == incompleteCommand[0])
			{
				results.push_back(uCommand.get());
			}
		}
	}
	return results;
}
} // namespace
#pragma endregion

#pragma region Implementation
void Init(LEContext& context)
{
	pContext = &context;
	Add<Help>();
	Add<LoadWorld>();
	Add<Quit>();
	Add<LogLevel>();
	Add<Resolution>();
#if ENABLED(TWEAKABLES)
	Add<Set>();
#endif
	Add<Unload>();
}

void Cleanup()
{
	commands.clear();
}

void AddCommand(UPtr<Command> uCommand)
{
	auto iter =
		std::find_if(commands.begin(), commands.end(), [&uCommand](const UPtr<Command>& uC) { return uCommand->m_name < uC->m_name; });
	commands.insert(iter, std::move(uCommand));
}

std::vector<LogLine> Execute(std::string_view query)
{
	std::string command;
	std::string params;
	std::string cleanedQuery = SplitQuery(query, command, params);
	std::vector<LogLine> ret;
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

AutoCompleteResults AutoComplete(std::string_view incompleteQuery)
{
	std::string incompleteCommand;
	std::string incompleteParams;
	std::string cleanedQuery = SplitQuery(incompleteQuery, incompleteCommand, incompleteParams);
	std::vector<Command*> matchedCommands = FindCommands(incompleteCommand, true);
	AutoCompleteResults results;
	if (!matchedCommands.empty())
	{
		// If exact match, build auto-compeleted params for the command
		if (matchedCommands.size() == 1)
		{
			std::vector<std::string> matchedParams = matchedCommands[0]->AutoCompleteParams(incompleteParams);
			for (auto& p : matchedParams)
			{
				results.params.emplace_back(std::move(p));
			}
			results.bCustomParam = matchedCommands[0]->m_bTakesCustomParam;
		}

		// If no params, return matchedCommands
		if (incompleteParams.empty())
		{
			for (auto command : matchedCommands)
			{
				results.queries.emplace_back(command->m_name);
			}
		}
		else
		{
			for (auto command : matchedCommands)
			{
				std::vector<std::string> matchedParams = command->AutoCompleteParams(incompleteParams);
				for (const auto& p : matchedParams)
				{
					std::string suffix = p.empty() ? "" : " " + p;
					results.queries.emplace_back(command->m_name + suffix);
				}
			}
		}
	}
	return results;
}
} // namespace Commands
#pragma endregion
} // namespace LittleEngine::Debug
#endif
