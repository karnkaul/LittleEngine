#include "stdafx.h"
#include "ConsoleInput.h"
#if ENABLED(CONSOLE)
#include "DebugConsole.h"
#include "DebugCommands.h"
#include "LittleEngine/Services/Services.h"

namespace LittleEngine
{
namespace Debug
{
ConsoleInput::ConsoleInput()
{
	m_token = Services::Engine()->Input()->RegisterSudo(std::bind(&ConsoleInput::OnInput, this, _1));
	m_liveLine.liveString.clear();
}

bool ConsoleInput::OnInput(const EngineInput::Frame& frame)
{
	if (frame.textInput.Contains(CONSOLE_KEY))
	{
		Console::g_bEnabled = !Console::g_bEnabled;
	}

	if (Console::g_bEnabled)
	{
		UpdateLiveLine(frame);
		return true;
	}

	return false;
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

String ConsoleInput::GetConsoleLiveLine(bool bShowCursor) const
{
	String cursor = bShowCursor ? "_" : "";
	return ">" + m_liveLine.liveString + cursor;
}

void ConsoleInput::UpdateLiveLine(const EngineInput::Frame& frame)
{
	bool bAppendText = false;
	const TextInput& textInput = frame.textInput;

	bool bNavigating = textInput.Contains(SpecialInputType::Up) || textInput.Contains(SpecialInputType::Down);
	if (!textInput.specials.empty() && !bNavigating)
	{
		bCyclingQueries = false;
		queryCache.ResetIter();
	}

	if (textInput.Contains(SpecialInputType::Enter))
	{
		m_query = m_liveLine.liveString;
		m_liveLine.Clear();
	}

	else if (frame.IsHeld(GameInputType::RB) && textInput.Contains(SpecialInputType::Insert))
	{
		m_liveLine.Append(frame.clipboard);
	}

	else if (textInput.Contains(SpecialInputType::Backspace))
	{
		m_liveLine.Backspace();
	}

	else if (textInput.Contains(SpecialInputType::Escape))
	{
		m_liveLine.Clear();
		LOG_D("Cleared");
	}

	else if (textInput.Contains(SpecialInputType::Tab))
	{
		static String previousLogText;
		if (!m_liveLine.liveString.empty())
		{
			Commands::AutoCompleteResults search = Commands::AutoComplete(m_liveLine.liveString);
			if (search.queries.empty())
			{
				Console::g_uLogBook->Append(LogLine(
					"Unrecognised query: \"" + m_liveLine.liveString + "\"", g_logWarningColour));
				return;
			}

			if (search.queries.size() == 1)
			{
				bool bSpace = search.bCustomParam || !search.params.empty();
				m_liveLine.liveString = search.queries[0] + (bSpace ? " " : "");
			}

			String logOutput;
			if (!search.params.empty())
			{
				for (const auto& params : search.params)
				{
					logOutput += ("\t" + params);
				}
			}
			else
			{
				for (const auto& query : search.queries)
				{
					logOutput += ("\t" + query);
				}
			}
			if (!logOutput.empty() && logOutput != previousLogText)
			{
				previousLogText = logOutput;
				Console::g_uLogBook->Append(LogLine(logOutput, g_logTextColour));
			}
		}
	}

	else if (textInput.Contains(SpecialInputType::Up))
	{
		if (!queryCache.IsEmpty())
		{
			if (!bCyclingQueries)
			{
				if (!m_liveLine.liveString.empty())
				{
					queryCache.PushFront(m_liveLine.liveString);
					queryCache.Increment();
				}
				bCyclingQueries = true;
			}
			else
			{
				queryCache.Increment();
			}
			m_liveLine.liveString = queryCache.Get();
		}
	}

	else if (textInput.Contains(SpecialInputType::Down))
	{
		if (!queryCache.IsEmpty())
		{
			if (!bCyclingQueries)
			{
				if (!m_liveLine.liveString.empty())
				{
					queryCache.PushFront(m_liveLine.liveString);
				}
				bCyclingQueries = true;
			}
			queryCache.Decrement();
			m_liveLine.liveString = queryCache.Get();
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

	else
	{
		bAppendText = true;
	}

	if (bAppendText && !textInput.text.empty())
	{
		String appendText = textInput.text;
		appendText.erase(std::remove(appendText.begin(), appendText.end(), CONSOLE_KEY), appendText.end());
		m_liveLine.Append(appendText);
	}
}
} // namespace Debug
} // namespace LittleEngine
#endif
