#include "stdafx.h"
#include <thread>
#include "SFEventLoop.h"
#include "SFWindow.h"
#include "SFWindowData.h"
#include "Logger.h"
#include "SFMLAPI/Input/SFEventHandler.h"
#include "SFMLAPI/Input/SFInputDataFrame.h"
#include "SFMLAPI/System/SFGameClock.h"
#include "SFMLAPI/Windowing/SFWindowData.h"

namespace LittleEngine
{
ASFEventLoop::ASFEventLoop()
{
	m_uSFEventHandler = MakeUnique<SFEventHandler>();
	m_uSFWindow = MakeUnique<SFWindow>();
}

ASFEventLoop::~ASFEventLoop() = default;

s32 ASFEventLoop::Run()
{
	// Construct Window
	m_uSFWindow->CreateWindow();
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
		{
			continue;
		}

		Time tickDT;
		if (!m_bPauseTicking)
		{
			Tick(currentTime, accumulator);
			if (m_bStopTicking)
			{
				continue;
			}

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
	LOG_I("[SFEventLoop] Event Loop terminated. Destroying SFWindow");

	m_uSFWindow = nullptr;
	
	return 0;
}

SFInputDataFrame ASFEventLoop::GetInputDataFrame() const
{
	return m_uSFEventHandler->GetFrameInputData();
}

void ASFEventLoop::PollEvents()
{
	SFWindowEventType windowEvent = m_uSFEventHandler->PollEvents(*m_uSFWindow);
	switch (windowEvent)
	{
	case SFWindowEventType::Closed:
		m_bStopTicking = true;
		break;

	case SFWindowEventType::LostFocus:
		if (m_bPauseOnFocusLoss)
		{
			OnPause(true);
		}
		break;

	case SFWindowEventType::GainedFocus:
		if (m_bPauseOnFocusLoss)
		{
			OnPause(false);
		}
		break;

	default:
		break;
	}
}

void ASFEventLoop::Tick(Time& outCurrentTime, Time& outAccumulator)
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
		{
			return;
		}
		m_elapsed += dt;
		outAccumulator -= dt;
	}
}

void ASFEventLoop::SleepForRestOfFrame(Time frameTime)
{
	Sleep(m_tickRate - frameTime);
}

void ASFEventLoop::Sleep(Time time)
{
	if (time.AsMilliseconds() > 0)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(time.AsMilliseconds()));
	}
}

void ASFEventLoop::OnPause(bool bPaused)
{
	m_bPauseTicking = bPaused;
}
} // namespace LittleEngine
