#include "stdafx.h"
#include <map>
#include "Utils/Utils.h"
#include "ConsoleCommands.h"
#include "Engine/Events/EventManager.h"
#include "Engine/Engine.h"
#include "Levels/Level.h"

namespace LittleEngine { namespace DebugConsole {
	namespace Commands {
		static std::vector<LogLine> GetAllCommands();
		static LittleEngine::Engine* pEngine;

		class Command {
		public:
			std::string name;

			virtual ~Command() {}
			
			std::vector<LogLine> Execute(const std::string& params) {
				executeResult.emplace_back(name + " " + params, LOG_QUERY_COLOUR);
				FillExecuteResult(params);
				return std::move(executeResult);
			}
			virtual std::vector<std::string> AutoCompleteParams(const std::string& incompleteParams) {
				std::vector<std::string> ret = { "" };
				return std::move(ret);
			}

		protected:
			std::vector<LogLine> executeResult;

			Command(const std::string& name) : name(std::move(name)) {}

			virtual void FillExecuteResult(const std::string& params) = 0;
		};

		class HelpCommand : public Command {
		public:
			HelpCommand() : Command("help") {}

			virtual void FillExecuteResult(const std::string& params) override {
				executeResult = GetAllCommands();
			}
		};

		class ParameterisedCommand : public Command {
		public:
			virtual void FillExecuteResult(const std::string& params) override final {
				if (params.empty()) {
					executeResult.emplace_back(OnEmptyParams());
				}

				else {
					auto search = paramCallbackMap.find(params);
					if (search != paramCallbackMap.end()) {
						search->second(executeResult);
					}
					else {
						executeResult.emplace_back("Unknown parameters: " + params, LOG_WARNING_COLOUR);
					}
				}
			}
			virtual std::vector<std::string> AutoCompleteParams(const std::string& incompleteParams) override final {
				std::vector<std::string> params;
				if (!incompleteParams.empty()) {
					for (const auto& p : paramCallbackMap) {
						if (p.first.find(incompleteParams) != std::string::npos && incompleteParams[0] == p.first[0]) {
							params.emplace_back(p.first);
						}
					}
				}
				return params;
			}

		protected:
			std::map<std::string, void(*)(std::vector<LogLine>&)> paramCallbackMap;

			ParameterisedCommand(const std::string& name) : Command(name) {}

			virtual LogLine OnEmptyParams() = 0;
		};

		class ShowCommand : public ParameterisedCommand {
		public:
			ShowCommand() : ParameterisedCommand("show") {
				paramCallbackMap.emplace("colliders", 
					[](std::vector<LogLine>& executeResult) {
						EventManager::Instance().Notify(GameEvent::DEBUG_SHOW_COLLIDERS);
						executeResult.emplace_back("Turned on Debug Colliders", LOG_TEXT_COLOUR);
					}
				);
			}

		protected:
			virtual LogLine OnEmptyParams() override {
				return LogLine("show what?", LOG_TEXT_COLOUR);
			}
		};

		class HideCommand : public ParameterisedCommand {
		public:
			HideCommand() : ParameterisedCommand("hide") {
				paramCallbackMap.emplace("colliders", 
					[](std::vector<LogLine>& executeResult) {
						EventManager::Instance().Notify(GameEvent::DEBUG_HIDE_COLLIDERS);
						executeResult.emplace_back("Turned off Debug Colliders", LOG_TEXT_COLOUR);
					}
				);
			}

		protected:
			virtual LogLine OnEmptyParams() override {
				return LogLine("hide what?", LOG_TEXT_COLOUR);
			}
		};

		class QuitCommand : public Command {
		public:
			QuitCommand() : Command("quit") {}

			virtual void FillExecuteResult(const std::string& params) override {
				executeResult.emplace_back("Quitting instantly", LOG_WARNING_COLOUR);
				Commands::pEngine->Quit();
			}
		};

		class LoadLevelCommand : public Command {
		public:
			LoadLevelCommand() : Command("loadlevel") {}

			virtual void FillExecuteResult(const std::string& params) override {
				if (params.empty()) {
					executeResult.emplace_back("Syntax: loadlevel <id>", LOG_TEXT_COLOUR);
					return;
				}

				int levelID = Strings::ToInt(params);
				if (levelID >= 0 && levelID <= Level::MAX_LEVEL_IDX) {
					executeResult.emplace_back("Loading Level ID: " + std::to_string(levelID), LOG_TEXT_COLOUR);
					Commands::pEngine->LoadLevel(static_cast<LevelID>(levelID));
				}
				else {
					executeResult.emplace_back("Invalid levelID " + params, LOG_WARNING_COLOUR);
				}
			}
		};

		static std::map<std::string, std::unique_ptr<Command>> s_commands;

		static std::vector<LogLine> GetAllCommands() {
			std::vector<LogLine> result;
			result.emplace_back("Registered commands:", LOG_TEXT_COLOUR);
			for (auto & s_command : s_commands) {
				result.emplace_back("\t" + s_command.second->name, LOG_TEXT_COLOUR);
			}
			return std::move(result);
		}

		static std::string StripPaddedSpaces(const std::string& input) {
			size_t firstNonSpaceIdx = 0;
			while (input[firstNonSpaceIdx] == ' ' && firstNonSpaceIdx < input.length()) {
				++firstNonSpaceIdx;
			}
			size_t lastNonSpaceIdx = input.length() - 1;
			while (input[lastNonSpaceIdx] == ' ' && lastNonSpaceIdx > firstNonSpaceIdx) {
				--lastNonSpaceIdx;
			}
			return input.substr(firstNonSpaceIdx, lastNonSpaceIdx - firstNonSpaceIdx + 1);
		}

		static void SplitQuery(std::string& outQuery, std::string& outCommand, std::string& outParams) {
			outQuery = StripPaddedSpaces(outQuery);
			outParams = "";
			if (outQuery.empty()) {
				outCommand = "";
				return;
			}

			size_t delimiterIdx = 0;
			while (outQuery[delimiterIdx] != ' ' && delimiterIdx != outQuery.length()) {
				++delimiterIdx;
			}
			outCommand = outQuery.substr(0, delimiterIdx);
			if (delimiterIdx + 1 < outQuery.length()) {
				outParams = outQuery.substr(delimiterIdx + 1, outQuery.length() - delimiterIdx);
			}
		}

		static std::vector<LogLine> ExecuteQuery(const std::string& command, const std::string& params) {
			std::vector<LogLine> ret;
			auto search = s_commands.find(command);
			if (search != s_commands.end()) {
				ret = search->second->Execute(params);
			}
			return std::move(ret);
		}

		static std::vector<Command*> FindCommands(const std::string& incompleteCommand) {
			std::vector<Command*> results;
			for (auto & s_command : s_commands) {
				std::string name = s_command.second->name;
				if (name.find(incompleteCommand) != std::string::npos) {
					results.push_back(&(*s_command.second));
				}
			}
			return std::move(results);
		}

		void Init(LittleEngine::Engine& engine) {
			pEngine = &engine;
			s_commands.emplace("help", std::make_unique<HelpCommand>());
			s_commands.emplace("show", std::make_unique<ShowCommand>());
			s_commands.emplace("hide", std::make_unique<HideCommand>());
			s_commands.emplace("quit", std::make_unique<QuitCommand>());
			s_commands.emplace("loadlevel", std::make_unique<LoadLevelCommand>());
		}

		std::vector<LogLine> Execute(const std::string & query) {
			std::string cleanedQuery(query);
			std::string command;
			std::string params;
			SplitQuery(cleanedQuery, command, params);
			std::vector<LogLine> ret;
			if (!cleanedQuery.empty()) {
				ret = Commands::ExecuteQuery(command, params);
				if (ret.empty()) {
					ret.emplace_back("Unrecognised command: " + command, LOG_WARNING_COLOUR);
				}
			}
			return std::move(ret);
		}

		std::vector<std::string> AutoComplete(const std::string& incompleteQuery) {
			std::string cleanedQuery(incompleteQuery);
			std::string incompleteCommand;
			std::string incompleteParams;
			SplitQuery(cleanedQuery, incompleteCommand, incompleteParams);

			std::vector<Command*> matchedCommands = FindCommands(incompleteCommand);
			std::vector<std::string> results;
			if (!matchedCommands.empty()) {
				// If no params, return matchedCommands
				if (incompleteParams.empty()) {
					for (auto command : matchedCommands) {
						results.push_back(command->name);
					}
				}
				else {
					for (auto command : matchedCommands) {
						std::vector<std::string> matchedParams = command->AutoCompleteParams(incompleteParams);
						for (const auto& p : matchedParams) {
							std::string suffix = p.empty() ? "" : " " + p;
							results.push_back(command->name + suffix);
						}
					}
				}
			}
			return std::move(results);
		}
	}
} }
