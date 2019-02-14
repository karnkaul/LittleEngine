#pragma once
#include "CoreTypes.h"
#if ENABLED(CONSOLE)
#include "LittleEngine/UI/UIText.h"
#include "SFMLAPI/Rendering/Colour.h"

namespace LittleEngine { namespace Debug {
	struct LogLine {
		String text;
		Colour colour;
		static constexpr u32 TEXT_SIZE = 19;

		LogLine(const String& text, Colour colour);
		UIText ToUIText() const;
	};

	class LogBook {
	private:
		const u32 m_lineCount;
		Vector<LogLine> m_logLines;
		Vector<LogLine>::reverse_iterator m_bottom;
		Vector<LogLine>::reverse_iterator m_top;

	public:
		LogBook(u32 lineCount);

		void Reset();
		void PageUp();
		void PageDown();

		Vector<LogLine> GetLogPage() const;
		void Append(Vector<LogLine>&& move);
		void Append(const LogLine& logLine);
	};

	struct LiveLine {
		String liveString;

		void Append(const String& newInput);
		void Backspace();
		void Clear();
	};
} }
#endif
