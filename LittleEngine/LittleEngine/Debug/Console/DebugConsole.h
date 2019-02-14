#pragma once
#include "StdTypes.h"
#if ENABLED(CONSOLE)
#include "LogLine.h"
#include "SFMLAPI/System/SFTime.h"
#include "SFMLAPI/Rendering/Colour.h"

namespace LittleEngine { namespace Debug {
	extern UByte g_logTextAlpha;
	extern Colour g_logTextColour;
	extern Colour g_logWarningColour;
	extern Colour g_liveHistoryColour;

	namespace Console {
		extern bool g_bEnabled;
		extern UPtr<LogBook> g_uLogBook;

		void Init();
		void Tick(Time dt);
		void Cleanup();
	}
} }
#endif
