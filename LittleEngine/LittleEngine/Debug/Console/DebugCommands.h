#pragma once
#include "CoreTypes.h"
#if ENABLED(CONSOLE)
#include "LogLine.h"

namespace LittleEngine { namespace Debug {
	namespace Commands {
		struct AutoCompleteResults {
			Vector<String> queries;
			Vector<String> params;
			bool bCustomParam = false;
		};

		void Init();
		Vector<LogLine> Execute(const String& query);
		AutoCompleteResults AutoComplete(const String& incompleteQuery);
	}
} }
#endif
