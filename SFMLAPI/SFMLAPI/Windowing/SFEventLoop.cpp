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
#if DEBUGGING
namespace
{
inline void ProfileFrameTime(Time frameElapsed, Time maxFrameTime)
{
	static const Time DILATED_TIME = Time::Milliseconds(250);
	static const u8 MAX_CONSECUTIVE = 2;
	static Time logTime = Time::Now() - Time::Milliseconds(300);
	static u8 consecutive = 0;
	if (frameElapsed > maxFrameTime)
	{
		++consecutive;
		if ((Time::Now() - logTime) > DILATED_TIME && consecutive > MAX_CONSECUTIVE)
		{
			f32 max = maxFrameTime.AsSeconds() * 1000.0f;
			f32 taken = frameElapsed.AsSeconds() * 1000.0f;
			LOG_E(
				"Game Loop is taking too long! Game time is inaccurate (slowed down) "
				"[max: "
				"%.2fms taken: %.2fms]",
				max, taken);
			logTime = Time::Now();
			consecutive = 0;
		}
	}
	else
	{
		consecutive = 0;
	}
}
} // namespace
#endif

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
		Time frameElapsed;
		PollEvents();
		// Break and exit if Window closed
		if (m_bStopTicking)
		{
			continue;
		}

		if (!m_bPauseTicking)
		{
			Integrate(currentTime, accumulator);
			FinishFrame();
			if (m_bStopTicking)
			{
				break;
			}
#if DEBUGGING
			ProfileFrameTime(Time::Now() - currentTime, m_tickRate);
#endif
			frameElapsed = Time::Now() - currentTime;
		}
		else
		{
			frameElapsed = Time::Milliseconds(m_tickRate.AsMilliseconds() * 0.25f);
		}
		Sleep(m_tickRate - frameElapsed);
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

void ASFEventLoop::Integrate(Time& outCurrentTime, Time& outAccumulator)
{
	Time dt = m_tickRate;
	Time newTime = Time::Now();
	Time frameTime = Time::Clamp(newTime - outCurrentTime, Time::Zero, m_maxFrameTime);
	outCurrentTime = newTime;

	outAccumulator += frameTime;
	if (outAccumulator >= dt)
	{
		PreTick();
	}
	while (outAccumulator >= dt)
	{
		GameClock::Tick(dt);
		bool bYield = Tick(dt);
		m_elapsed += dt;
		if (bYield)
		{
			outAccumulator = Time::Zero;
			LOG_D("[EventLoop] Yielded integration");
			return;
		}
		outAccumulator -= dt;
	}
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
