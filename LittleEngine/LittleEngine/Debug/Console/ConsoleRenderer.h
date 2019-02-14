#pragma once
#include "CoreTypes.h"
#if ENABLED(CONSOLE)
#include "DebugConsole.h"
#include "LittleEngine/UI/UIElement.h"
#include "SFMLAPI/Rendering/Colour.h"

namespace LittleEngine { namespace Debug {
	class ConsoleRenderer {
	public:
		Vector2 maxPadding;
		Vector2 minPadding;

		UPtr<UIElement> m_uBG;
		UPtr<UIElement> m_uSeparator;

		UPtr<UIElement> m_uLiveText;
		Vector<UPtr<UIElement>> m_uLogTexts;

		Colour m_liveTextColour = Colour(255, 255, 255, g_logTextAlpha);
		Colour m_bgColour = Colour(100, 50, 100, 100);
		FontAsset* m_pFont;
		u32 m_textSize = LogLine::TEXT_SIZE;
		u32 m_logLinesCount;

		ConsoleRenderer();

		void Tick(Time dt);
		void SetLiveString(const String& text);
		void UpdateLog(const Vector<LogLine>& logLines);
	};
} }
#endif
