#pragma once
#include "Core/CoreTypes.h"
#if ENABLED(CONSOLE)
#include "SFMLAPI/Rendering/Colour.h"

namespace LittleEngine
{
struct UIText;

namespace Debug
{
struct LogLine
{
	std::string text;
	Colour colour;
	static constexpr u16 TEXT_SIZE = 19;

	LogLine() = default;
	LogLine(std::string text, Colour colour);
	UIText ToUIText() const;
};

class LogBook
{
private:
	const u32 m_lineCount;
	std::vector<LogLine> m_logLines;
	std::vector<LogLine>::reverse_iterator m_bottom;
	std::vector<LogLine>::reverse_iterator m_top;

public:
	LogBook(u32 lineCount);
	~LogBook();

	void Reset();
	void PageUp();
	void PageDown();

	std::vector<LogLine> LogPage() const;
	void Append(std::vector<LogLine>&& move);
	void Append(LogLine logLine);
};
} // namespace Debug
} // namespace LittleEngine
#endif
