#pragma once
#include "CoreTypes.h"
#if ENABLED(CONSOLE)
#include "LogLine.h"

namespace LittleEngine { namespace Debug {
	namespace Commands {
		struct AutoCompleteResults {
			Vec<String> queries;
			Vec<String> params;
			bool bCustomParam = false;
		};

		void Init();
		Vec<LogLine> Execute(const String& query);
		AutoCompleteResults AutoComplete(const String& incompleteQuery);
	}
} }
#endif
