#include "DebugConsole.h"
#if ENABLED(CONSOLE)
#include "Core/Logger.h"
#include "ConsoleInput.h"
#include "ConsoleRenderer.h"
#include "DebugCommands.h"
#include "LogLine.h"
#include "Framework/Utility/Debug/RenderStatsRenderer.h"
#include "Engine/Input/LEInput.h"
#include "Engine/Context/LEContext.h"
#include "Model/GameManager.h"

namespace LittleEngine::Debug
{
UByte g_logTextAlpha = 200;
Colour g_logTextColour = Colour(220, 220, 220, g_logTextAlpha);
Colour g_logWarningColour = Colour(255, 255, 50, g_logTextAlpha);
Colour g_liveHistoryColour = Colour(150, 150, 150, g_logTextAlpha);

namespace Console
{
bool g_bEnabled = false;
UPtr<LogBook> g_uLogBook;

bool bQuit = false;

void Quit()
{
	bQuit = true;
}
} // namespace Console

namespace
{
UPtr<class ConsoleImpl> uConsole;
LEContext* pContext = nullptr;

class ConsoleImpl
{
private:
	ConsoleInput m_input;
	ConsoleRenderer m_renderer;
	RenderStatsRenderer m_renderStats;
#if defined(DEBUGGING)
	VersionRenderer m_version;
#endif

	Time m_elapsed;
	Time m_cursorFreq = Time::Milliseconds(225);
	bool m_bShowCursor = true;

public:
	ConsoleImpl();
	~ConsoleImpl();

	void Tick(Time dt);
};

ConsoleImpl::ConsoleImpl() : m_input(*pContext), m_renderer(*pContext), m_renderStats(*pContext), m_version(*pContext)
{
	LOG_D("Debug Console initialised");
	Console::g_uLogBook = std::make_unique<LogBook>(m_renderer.m_logLinesCount);
}

ConsoleImpl::~ConsoleImpl()
{
	LOG_D("Debug Console uninitialised");
}

void ConsoleImpl::Tick(Time dt)
{
	m_elapsed += dt;
	m_renderer.Tick(dt);
	if (Console::g_bEnabled)
	{
		m_input.Update();

		// Live Line
		if (m_elapsed >= m_cursorFreq)
		{
			m_bShowCursor = !m_bShowCursor;
			m_elapsed = Time::Zero;
		}
		m_renderer.SetLiveString(m_input.m_keyboard.LiveString(), m_input.m_keyboard.m_liveLine.CursorNPos(), m_bShowCursor);
		// Log Lines
		m_renderer.UpdateLog(Console::g_uLogBook->LogPage());
	}
	m_renderStats.Update();
}
} // namespace

void Console::Init()
{
	Assert(g_pGameManager, "GameManager is null!");
	pContext = g_pGameManager->Context();
	bQuit = false;
	uConsole = std::make_unique<ConsoleImpl>();
	Commands::Init(*pContext);
}

void Console::Tick(Time dt)
{
	if (bQuit)
	{
		pContext->Terminate();
	}
	if (uConsole)
	{
		uConsole->Tick(dt);
	}
}

void Console::Cleanup()
{
	Commands::Cleanup();
	uConsole = nullptr;
	pContext = nullptr;
}
} // namespace LittleEngine::Debug
#endif
