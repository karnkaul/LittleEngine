#pragma once
#include "Core/StdTypes.h"
#if ENABLED(CONSOLE)
#include "Core/SimpleTime.h"
#include "SFMLAPI/Rendering/Colour.h"

namespace LittleEngine
{
class LEContext;

namespace Debug
{
class LogBook;

extern UByte g_logTextAlpha;
extern Colour g_logTextColour;
extern Colour g_logWarningColour;
extern Colour g_liveHistoryColour;

namespace Console
{
extern bool g_bEnabled;
extern UPtr<LogBook> g_uLogBook;

void Init(LEContext& context);
void Tick(Time dt);
void Cleanup();
} // namespace Console
} // namespace Debug
} // namespace LittleEngine
#endif
