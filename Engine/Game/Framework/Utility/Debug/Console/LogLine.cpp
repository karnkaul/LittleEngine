#include "LogLine.h"
#include "Game/Model/UI/UIText.h"
#if ENABLED(CONSOLE)

namespace LittleEngine::Debug
{
extern Colour g_logTextColour;

LogLine::LogLine(std::string text, Colour colour) : text(std::move(text)), colour(colour) {}

UIText LogLine::ToUIText() const
{
	return UIText(text, TEXT_SIZE, colour);
}

LogBook::LogBook(u32 lineCount) : m_lineCount(lineCount)
{
	Reset();
}

LogBook::~LogBook() = default;

void LogBook::Reset()
{
	m_bottom = m_logLines.rbegin();
	m_top = m_bottom;
	for (u32 lines = 0; lines < m_lineCount && m_top != m_logLines.rend(); ++lines)
	{
		++m_top;
	}
}

void LogBook::PageUp()
{
	if (m_logLines.size() < m_lineCount)
	{
		return;
	}
	for (u32 line = 0; line < m_lineCount - 1 && m_top != m_logLines.rend(); ++line)
	{
		++m_top;
		++m_bottom;
	}
}

void LogBook::PageDown()
{
	if (m_logLines.size() < m_lineCount)
	{
		return;
	}
	for (u32 line = 0; line < m_lineCount - 1 && m_bottom != m_logLines.rbegin(); ++line)
	{
		--m_top;
		--m_bottom;
	}
}

std::vector<LogLine> LogBook::LogPage() const
{
	std::vector<LogLine> ret;
	std::copy(m_bottom, m_top, std::back_inserter(ret));
	return ret;
}

void LogBook::Append(std::vector<LogLine>&& move)
{
	std::move(move.begin(), move.end(), std::back_inserter(m_logLines));
	Reset();
}

void LogBook::Append(LogLine logLine)
{
	m_logLines.emplace_back(std::move(logLine));
	Reset();
}
} // namespace LittleEngine::Debug
#endif
