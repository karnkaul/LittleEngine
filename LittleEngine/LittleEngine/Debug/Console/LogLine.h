#pragma once
#include "CoreTypes.h"
#if ENABLED(CONSOLE)
#include "LittleEngine/UI/UIText.h"
#include "SFMLAPI/Rendering/Colour.h"

namespace LittleEngine
{
namespace Debug
{
struct LogLine
{
	String text;
	Colour colour;
	static constexpr u32 TEXT_SIZE = 19;

	LogLine(const String& text, Colour colour);
	UIText ToUIText() const;
};

class LogBook
{
private:
	const u32 m_lineCount;
	Vec<LogLine> m_logLines;
	Vec<LogLine>::reverse_iterator m_bottom;
	Vec<LogLine>::reverse_iterator m_top;

public:
	LogBook(u32 lineCount);

	void Reset();
	void PageUp();
	void PageDown();

	Vec<LogLine> GetLogPage() const;
	void Append(Vec<LogLine>&& move);
	void Append(const LogLine& logLine);
};
} // namespace Debug
} // namespace LittleEngine
#endif
