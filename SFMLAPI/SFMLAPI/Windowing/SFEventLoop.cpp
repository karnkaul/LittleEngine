#include "stdafx.h"
#include "SFEventLoop.h"
#include "SFWindow.h"
#include "SFMLAPI/System/SFGameClock.h"
#include "Logger.h"

namespace LittleEngine
{
SFEventLoop::~SFEventLoop() = default;

s32 SFEventLoop::Run()
{
	// Construct Window
	m_uSFWindow = MakeUnique<SFWindow>(m_windowData);
	LOG_I("[SFEventLoop] Running");

	PreRun();

	GameClock::Reset();
	Time t;
	Time accumulator;
	Time currentTime = Time::Now();
	while (m_uSFWindow && !m_bStopTicking)
	{
		// Break and exit if Window closed
		PollEvents();
		if (m_bStopTicking)
			continue;

		Time tickDT;
		if (!m_bPauseTicking)
		{
			Tick(currentTime, accumulator);
			if (m_bStopTicking)
				continue;

			PostTick();
			tickDT = Time::Now() - currentTime;
		}
		else
		{
			tickDT = Time::Milliseconds(m_tickRate.AsMilliseconds() * 0.25f);
		}
		SleepForRestOfFrame(tickDT);
	}

	PostRun();
	LOG_I("[SFEventLoop] Activating SFWindow on this thread, destroying SFWindow");

	m_uSFWindow->setActive(true);
	m_uSFWindow->close();
	m_uSFWindow = nullptr;
	PostWindowDestruct();

	return 0;
}

SFInputDataFrame SFEventLoop::GetInputDataFrame() const
{
	return m_sfEventHandler.GetFrameInputData();
}

void SFEventLoop::PollEvents()
{
	SFWindowEventType windowEvent = m_sfEventHandler.PollEvents(*m_uSFWindow);
	switch (windowEvent)
	{
	case SFWindowEventType::Closed:
		m_bStopTicking = true;
		break;

	case SFWindowEventType::LostFocus:
		if (m_bPauseOnFocusLoss)
			OnPause(true);
		break;

	case SFWindowEventType::GainedFocus:
		if (m_bPauseOnFocusLoss)
			OnPause(false);
		break;

	default:
		break;
	}
}

void SFEventLoop::Tick(Time& outCurrentTime, Time& outAccumulator)
{
	Time dt = m_tickRate;
	Time newTime = Time::Now();
	Time frameTime = Time::Clamp(newTime - outCurrentTime, Time::Zero, m_maxFrameTime);
	outCurrentTime = newTime;

	outAccumulator += frameTime;
	while (outAccumulator >= dt)
	{
		GameClock::Tick(dt);
		Tick(dt);
		if (m_bStopTicking)
			return;
		m_elapsed += dt;
		outAccumulator -= dt;
	}
}

void SFEventLoop::SleepForRestOfFrame(Time frameTime)
{
	s32 surplus = (m_tickRate - frameTime).AsMilliseconds();
	if (Core::g_MinLogSeverity >= Core::LogSeverity::HOT)
	{
		StringStream s;
		s << "[SFEventLoop] Frame Update Complete. Time taken: " << frameTime.AsMilliseconds()
		  << " Surplus: " << surplus;
		LOG_H("[SFEventLoop] Frame Update Complete. Time taken: %d Surplus: %d",
			  frameTime.AsMilliseconds(), surplus);
		if (surplus > 0)
			sf::sleep(sf::milliseconds(surplus));
	}
}

void SFEventLoop::Sleep(Time time)
{
	sf::sleep(sf::milliseconds(time.AsMilliseconds()));
}

void SFEventLoop::PreRun()
{
}
void SFEventLoop::PostRun()
{
}
void SFEventLoop::PostWindowDestruct()
{
}
void SFEventLoop::Tick(Time)
{
}
void SFEventLoop::PostTick()
{
}
void SFEventLoop::OnPause(bool bPaused)
{
	m_bPauseTicking = bPaused;
}
} // namespace LittleEngine
