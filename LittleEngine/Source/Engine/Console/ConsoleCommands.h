#pragma once
#include <vector>
#include <string>
#include "LogLine.hpp"

namespace LittleEngine { namespace DebugConsole {
	namespace Commands {
		void Init(class Engine& engine);
		std::vector<LogLine> Execute(const std::string& query);
		std::vector<std::string> AutoComplete(const std::string& incompleteQuery);
	}
} }
