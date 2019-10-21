#pragma once
#include "Core/CoreTypes.h"
#if ENABLED(CONSOLE)
#include "DebugConsole.h"
#include "SFMLAPI/Rendering/Colour.h"

namespace LittleEngine
{
class UIElement;
class LEContext;

namespace Debug
{
struct LogLine;

class ConsoleRenderer final
{
public:
	Vector2 maxPadding;
	Vector2 minPadding;

	std::unique_ptr<UIElement> m_uBG;
	std::unique_ptr<UIElement> m_uSeparator;

	std::unique_ptr<UIElement> m_uCarat;
	std::unique_ptr<UIElement> m_uLiveText;
	std::unique_ptr<UIElement> m_uCursor;
	std::vector<std::unique_ptr<UIElement>> m_uLogTexts;

	Colour m_liveTextColour = Colour(255, 255, 255, g_logTextAlpha);
	Colour m_bgColour = Colour(50, 25, 50, 225);
	Fixed m_cursorNPos;
	u16 m_textSize;
	u32 m_logLinesCount;

public:
	ConsoleRenderer(LEContext& context);
	~ConsoleRenderer();

	void Tick(Time dt);
	void SetLiveString(std::string text, Fixed cursorNPos, bool bShowCursor);
	void UpdateLog(std::vector<LogLine> logLines);
};
} // namespace Debug
} // namespace LittleEngine
#endif
