#include "stdafx.h"
#include "ConsoleInput.h"
#if ENABLED(CONSOLE)
#include "DebugConsole.h"
#include "DebugCommands.h"
#include "LogLine.h"
#include "LittleEngine/Context/LEContext.h"

namespace LittleEngine::Debug
{
ConsoleInput::ConsoleInput(LEContext& context) : m_pContext(&context)
{
	m_token = m_pContext->Input()->RegisterSudo(
		[&](const LEInput::Frame& frame) -> bool { return OnInput(frame); });
}

bool ConsoleInput::OnInput(const LEInput::Frame& frame)
{
	bool bWasEnabled = Console::g_bEnabled;
	if (frame.textInput.ContainsChar(CONSOLE_KEY))
	{
		Console::g_bEnabled = !Console::g_bEnabled;
	}

	if (Console::g_bEnabled)
	{
		UpdateLiveLine(frame);
		return true;
	}

	return bWasEnabled;
}

void ConsoleInput::Update()
{
	if (!m_query.empty())
	{
		m_queryCache.PushBack(m_query);
		Vec<LogLine> logAppend = Commands::Execute(m_query);
		Console::g_uLogBook->Append(std::move(logAppend));
		m_query.clear();
	}
}

void ConsoleInput::UpdateLiveLine(const LEInput::Frame& frame)
{
	const TextInput& textInput = frame.textInput;

	bool bNavigating = textInput.ContainsKey(KeyCode::Up) || textInput.ContainsKey(KeyCode::Down);
	bool bResetQuery = false;

	if (frame.IsPressed(KeyCode::Enter))
	{
		m_query = m_keyboard.m_liveLine.liveString;
		m_keyboard.Clear();
		bResetQuery = true;
	}

	else if (frame.IsPressed(KeyCode::Tab))
	{
		if (!m_keyboard.m_liveLine.liveString.empty())
		{
			Commands::AutoCompleteResults search = Commands::AutoComplete(m_keyboard.m_liveLine.liveString);
			if (search.queries.empty())
			{
				Console::g_uLogBook->Append(LogLine(
					"Unrecognised query: \"" + m_keyboard.m_liveLine.liveString + "\"", g_logWarningColour));
				return;
			}

			if (search.queries.size() == 1)
			{
				bool bSpace = search.bCustomParam || !search.params.empty();
				m_keyboard.m_liveLine.Set(search.queries[0] + (bSpace ? " " : ""));
			}

			const Vec<String>& vec = search.params.empty() ? search.queries : search.params;
			static const u8 columns = 3;
			static const u8 columnWidth = 20;
			String line;
			Vec<LogLine> logLines;
			u8 column = 0;
			for (const auto& autoCompleted : vec)
			{
				u8 pad = 0;
				u8 span = (autoCompleted.size() + 1) / columnWidth;
				u8 overflow = span > 0 ? autoCompleted.size() % columnWidth : autoCompleted.size();
				pad = columnWidth - overflow;
				column += span;
				line += (autoCompleted + String(pad, ' '));
				++column;
				if (column >= columns)
				{
					line = "\t" + std::move(line);
					logLines.emplace_back(std::move(line), g_logTextColour);
					column = 0;
				}
			}
			if (!line.empty())
			{
				line = "\t" + std::move(line);
				logLines.emplace_back(std::move(line), g_logTextColour);
			}
			if (!logLines.empty())
			{
				Console::g_uLogBook->Append(LogLine());
				for (const auto& logLine : logLines)
				{
					Console::g_uLogBook->Append(std::move(logLine));
				}
			}
		}
		bResetQuery = true;
	}

	else if (frame.IsPressed(KeyCode::Down))
	{
		if (!m_queryCache.IsEmpty())
		{
			if (!bCyclingQueries)
			{
				if (!m_keyboard.m_liveLine.liveString.empty())
				{
					m_queryCache.PushBack(m_keyboard.m_liveLine.liveString);
					m_queryCache.Increment();
				}
				bCyclingQueries = true;
			}
			else
			{
				m_queryCache.Increment();
			}
			m_keyboard.m_liveLine.Set(m_queryCache.GetVal());
		}
	}

	else if (frame.IsPressed(KeyCode::Up))
	{
		if (!m_queryCache.IsEmpty())
		{
			if (!bCyclingQueries)
			{
				if (!m_keyboard.m_liveLine.liveString.empty())
				{
					m_queryCache.PushBack(m_keyboard.m_liveLine.liveString);
				}
				bCyclingQueries = true;
			}
			m_queryCache.Decrement();
			m_keyboard.m_liveLine.Set(m_queryCache.GetVal());
		}
	}

	else if (frame.IsPressed(KeyCode::PageUp))
	{
		Console::g_uLogBook->PageUp();
	}

	else if (frame.IsPressed(KeyCode::PageDown))
	{
		Console::g_uLogBook->PageDown();
	}

	else if (!frame.textInput.ContainsChar(CONSOLE_KEY))
	{
		m_keyboard.Update(frame);
	}

	if (bResetQuery && !bNavigating)
	{
		bCyclingQueries = false;
		m_queryCache.Reset();
	}
}
} // namespace LittleEngine::Debug
#endif
