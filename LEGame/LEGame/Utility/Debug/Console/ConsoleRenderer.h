#pragma once
#include "Core/CoreTypes.h"
#if ENABLED(CONSOLE)
#include "DebugConsole.h"
#include "SFMLAPI/Rendering/Colour.h"

namespace LittleEngine
{
class UIElement;
class FontAsset;
class LEContext;

namespace Debug
{
struct LogLine;

class ConsoleRenderer final
{
public: 
	Vector2 maxPadding;
	Vector2 minPadding;

	UPtr<UIElement> m_uBG;
	UPtr<UIElement> m_uSeparator;

	UPtr<UIElement> m_uCarat;
	UPtr<UIElement> m_uLiveText;
	UPtr<UIElement> m_uCursor;
	Vec<UPtr<UIElement>> m_uLogTexts;

	Colour m_liveTextColour = Colour(255, 255, 255, g_logTextAlpha);
	Colour m_bgColour = Colour(100, 50, 100, 100);
	FontAsset* m_pFont;
	Fixed m_cursorNPos;
	u32 m_textSize;
	u32 m_logLinesCount;

public:
	ConsoleRenderer(LEContext& context);
	~ConsoleRenderer();

	void Tick(Time dt);
	void SetLiveString(String text, Fixed cursorNPos, bool bShowCursor);
	void UpdateLog(Vec<LogLine> logLines);
};
} // namespace Debug
} // namespace LittleEngine
#endif
