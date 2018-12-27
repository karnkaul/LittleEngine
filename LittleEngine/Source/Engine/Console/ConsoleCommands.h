#pragma once
#include <vector>
#include <string>
#include "LogLine.hpp"

namespace LittleEngine { 
	class Engine;
namespace DebugConsole {
	namespace Commands {
		using StringVec = std::vector<std::string>;
		
		struct AutoCompleteResults {
			StringVec queries;
			StringVec params;
		};

		void Init(Engine& engine);
		std::vector<LogLine> Execute(const std::string& query);
		AutoCompleteResults AutoComplete(const std::string& incompleteQuery);
	}
} }
