#pragma once
#include "CoreTypes.h"
#include "SimpleTime.h"

namespace LittleEngine
{
// \brief: Base class that runs Event Loop on this thread (blocking)
// Derived classes provide custom game loops
class ASFEventLoop
{
protected:
	UPtr<class SFEventHandler> m_uSFEventHandler;
	UPtr<class SFWindow> m_uSFWindow;
	// Events will be polled and Tick() called at this rate
	Time m_tickRate = Time::Seconds(1.0f / 45.0f);
	// No more than this much frame time will be ticked per frame
	Time m_maxFrameTime = Time::Milliseconds(25);
	bool m_bStopTicking = false;
	bool m_bPauseOnFocusLoss = true;

private:
	Time m_elapsed;
	bool m_bPauseTicking = false;

public:
	ASFEventLoop();
	virtual ~ASFEventLoop();

	// Blocks current thread and starts event loop on it
	s32 Run();

protected:
	struct SFInputDataFrame GetInputDataFrame() const;
	void Sleep(Time time);

private:
	void PollEvents();
	void Tick(Time& outCurrentTime, Time& outAccumulator);
	void SleepForRestOfFrame(Time frameTime);

protected:
	virtual void OnPause(bool bPaused);

private:
	// Called after Window creation, before starting event loop
	virtual void PreRun() = 0;
	// Called after event loop, before Window destruction
	virtual void PostRun() = 0;
	// Fixed time slice Tick
	virtual void Tick(Time dt) = 0;
	// Called once per frame; use to swap buffers etc
	virtual void PostTick() = 0;
};
} // namespace LittleEngine
