#pragma once
#include "CoreTypes.h"
#include "SFWindowData.h"
#include "SFMLAPI/System/SFTime.h"
#include "SFMLAPI/Input/SFEventHandler.h"

namespace LittleEngine
{
// \brief: Base class that runs Event Loop on this thread (blocking)
// Derived classes provide custom game loops
class SFEventLoop
{
protected:
	SFWindowData m_windowData;
	SFEventHandler m_sfEventHandler;
	// Events will be polled and Tick() called at this rate
	Time m_tickRate = Time::Seconds(1.0f / 45.0f);
	// No more than this much frame time will be ticked per frame
	Time m_maxFrameTime = Time::Milliseconds(25);
	UPtr<class SFWindow> m_uSFWindow;
	bool m_bStopTicking = false;
	bool m_bPauseOnFocusLoss = true;
private:
	Time m_elapsed;
	bool m_bPauseTicking = false;

public:
	virtual ~SFEventLoop();

	// Blocks current thread and starts event loop on it
	s32 Run();

protected:
	SFInputDataFrame GetInputDataFrame() const;
	void Sleep(Time time);

private:
	void PollEvents();
	void Tick(Time& outCurrentTime, Time& outAccumulator);
	void SleepForRestOfFrame(Time frameTime);

protected:
	// Called after Window creation, before starting event loop
	virtual void PreRun();
	// Called after event loop, before Window destruction
	virtual void PostRun();
	// Called after event loop, after Window destruction
	virtual void PostWindowDestruct();
	// Fixed time slice Tick
	virtual void Tick(Time dt);
	// Called once per frame; use to swap buffers etc
	virtual void PostTick();

	virtual void OnPause(bool bPaused);
};
} // namespace LittleEngine
