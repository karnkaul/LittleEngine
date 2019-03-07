#include "stdafx.h"
#include "DebugConsole.h"
#if ENABLED(CONSOLE)
#include "DebugCommands.h"
#include "ConsoleInput.h"
#include "ConsoleRenderer.h"
#include "RenderStatsRenderer.h"
#include "CircularList.hpp"
#include "LogLine.h"
#include "LittleEngine/Input/EngineInput.h"
#include "LittleEngine/Services/Services.h"
#include "SFMLAPI/Rendering/SFRenderer.h"
#include "Logger.h"

namespace LittleEngine
{
namespace Debug
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

class ConsoleImpl
{
private:
	ConsoleInput m_input;
	ConsoleRenderer m_renderer;
	RenderStatsRenderer m_renderStats;
#if !SHIPPING
	VersionRenderer m_version;
#endif

	Time m_elapsed;
	s32 m_cursorFrequencyMS = 200;
	bool m_bShowCursor;

public:
	ConsoleImpl();
	~ConsoleImpl();

	void Tick(Time dt);
};

ConsoleImpl::ConsoleImpl()
{
	LOG_D("Debug Console initialised");
	Console::g_uLogBook = MakeUnique<LogBook>(m_renderer.m_logLinesCount);
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
		if (m_elapsed.AsMilliseconds() >= m_cursorFrequencyMS)
		{
			m_bShowCursor = !m_bShowCursor;
			m_elapsed = Time::Zero;
		}
		m_renderer.SetLiveString(m_input.GetConsoleLiveLine(m_bShowCursor));
		// Log Lines
		m_renderer.UpdateLog(Console::g_uLogBook->GetLogPage());
	}
	m_renderStats.Tick(dt);
}
} // namespace

void Console::Init()
{
	bQuit = false;
	uConsole = MakeUnique<ConsoleImpl>();
	Commands::Init();
}

void Console::Tick(Time dt)
{
	if (bQuit)
		Services::Engine()->Terminate();
	if (uConsole)
		uConsole->Tick(dt);
}

void Console::Cleanup()
{
	uConsole = nullptr;
}
} // namespace Debug
} // namespace LittleEngine
#endif
