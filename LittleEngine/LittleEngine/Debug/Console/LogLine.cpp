#include "stdafx.h"
#include "LogLine.h"
#if ENABLED(CONSOLE)

namespace LittleEngine { namespace Debug {
	LogLine::LogLine(const String& text, Colour colour) : text(text), colour(colour) {}

	UIText LogLine::ToUIText() const {
		return UIText(text, TEXT_SIZE, colour);
	}

	void LiveLine::Append(const String& newInput) {
		if (!newInput.empty()) liveString += newInput;
	}

	void LiveLine::Backspace() {
		liveString = liveString.substr(0, liveString.size() - 1);
	}

	void LiveLine::Clear() {
		liveString.clear();
	}

	LogBook::LogBook(u32 lineCount) : m_lineCount(lineCount) {
		Reset();
	}

	void LogBook::Reset() {
		m_bottom = m_logLines.rbegin();
		m_top = m_bottom;
		for (u32 lines = 0; lines < m_lineCount && m_top != m_logLines.rend(); ++lines) {
			++m_top;
		}
	}

	void LogBook::PageUp() {
		if (m_logLines.size() < m_lineCount) return;
		for (u32 line = 0; line < m_lineCount - 1 && m_top != m_logLines.rend(); ++line) {
			++m_top;
			++m_bottom;
		}
	}

	void LogBook::PageDown() {
		if (m_logLines.size() < m_lineCount) return;
		for (u32 line = 0; line < m_lineCount - 1 && m_bottom != m_logLines.rbegin(); ++line) {
			--m_top;
			--m_bottom;
		}
	}

	Vector<LogLine> LogBook::GetLogPage() const {
		Vector<LogLine> ret;
		std::copy(m_bottom, m_top, std::back_inserter(ret));
		return ret;
	}

	void LogBook::Append(Vector<LogLine>&& move) {
		std::move(move.begin(), move.end(), std::back_inserter(m_logLines));
		Reset();
	}

	void LogBook::Append(const LogLine & logLine) {
		m_logLines.emplace_back(logLine);
		Reset();
	}
} }
#endif
