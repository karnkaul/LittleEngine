#include "stdafx.h"
#include "ConsoleInput.h"
#if ENABLED(CONSOLE)
#include "DebugConsole.h"
#include "DebugCommands.h"
#include "LogLine.h"
#include "LittleEngine/Context/LEContext.h"

namespace LittleEngine
{
namespace Debug
{
ConsoleInput::ConsoleInput(LEContext& context) : m_pContext(&context)
{
	m_token = m_pContext->Input()->RegisterSudo(
		[&](const LEInput::Frame& frame) -> bool { return OnInput(frame); });
}

bool ConsoleInput::OnInput(const LEInput::Frame& frame)
{
	bool bWasEnabled = Console::g_bEnabled;
	if (frame.textInput.Contains(CONSOLE_KEY))
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
		queryCache.PushFront(m_query);
		Vec<LogLine> logAppend = Commands::Execute(m_query);
		Console::g_uLogBook->Append(std::move(logAppend));
		m_query.clear();
	}
}

void ConsoleInput::UpdateLiveLine(const LEInput::Frame& frame)
{
	const TextInput& textInput = frame.textInput;

	bool bNavigating = textInput.Contains(SpecialInputType::Up) || textInput.Contains(SpecialInputType::Down);
	if (!textInput.specials.empty() && !bNavigating)
	{
		bCyclingQueries = false;
		queryCache.ResetIter();
	}

	if (textInput.Contains(SpecialInputType::Enter))
	{
		m_query = m_keyboard.m_liveLine.liveString;
		m_keyboard.Clear();
	}

	else if (textInput.Contains(SpecialInputType::Tab))
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
	}

	else if (textInput.Contains(SpecialInputType::Up))
	{
		if (!queryCache.IsEmpty())
		{
			if (!bCyclingQueries)
			{
				if (!m_keyboard.m_liveLine.liveString.empty())
				{
					queryCache.PushFront(m_keyboard.m_liveLine.liveString);
					queryCache.Increment();
				}
				bCyclingQueries = true;
			}
			else
			{
				queryCache.Increment();
			}
			m_keyboard.m_liveLine.Set(queryCache.Get());
		}
	}

	else if (textInput.Contains(SpecialInputType::Down))
	{
		if (!queryCache.IsEmpty())
		{
			if (!bCyclingQueries)
			{
				if (!m_keyboard.m_liveLine.liveString.empty())
				{
					queryCache.PushFront(m_keyboard.m_liveLine.liveString);
				}
				bCyclingQueries = true;
			}
			queryCache.Decrement();
			m_keyboard.m_liveLine.Set(queryCache.Get());
		}
	}

	else if (textInput.Contains(SpecialInputType::PageUp))
	{
		Console::g_uLogBook->PageUp();
	}

	else if (textInput.Contains(SpecialInputType::PageDown))
	{
		Console::g_uLogBook->PageDown();
	}

	else if (!frame.textInput.Contains(CONSOLE_KEY))
	{
		m_keyboard.Update(frame);
	}
}
} // namespace Debug
} // namespace LittleEngine
#endif
