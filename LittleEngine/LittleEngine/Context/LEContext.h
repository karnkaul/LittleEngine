#pragma once
#include "Core/CoreTypes.h"
#include "SFMLAPI/Input/SFInputHandler.h"
#include "SFMLAPI/Viewport/SFViewportData.h"

namespace LittleEngine
{
struct LEContextData
{
	SFViewportData viewportData;
	Time tickRate = Time::Seconds(1.0f / 60.0f);
	Time maxFrameTime = Time::Milliseconds(50);
	Time renderThreadStartDelay = Time::Milliseconds(10);
	bool bPauseOnFocusLoss = false;
	bool bRenderThread = true;
};

class LEContext final
{
private:
	LEContextData m_data;
	SFInputHandler m_inputHandler;
	UPtr<class LEInput> m_uInput;
	UPtr<class SFViewport> m_uViewport;
	UPtr<class LERenderer> m_uRenderer;
	bool m_bTerminating = false;
	bool m_bWaitingToTerminate = false;
	bool m_bPauseTicking = false;

	std::optional<SFViewportStyle> m_oNewViewportStyle;
	std::optional<SFViewportSize*> m_oNewViewportSize;
	
	
public:
	LEContext(LEContextData data);
	~LEContext();

	bool IsTerminating() const;
	bool IsPaused() const;

	LEInput* Input() const;
	LERenderer* Renderer() const;
	
	Vector2 GetViewSize() const;
	Time GetMaxFrameTime() const;

	bool TrySetViewportSize(u32 height);
	void SetWindowStyle(SFViewportStyle newStyle);
	void Terminate();

	
	void PollInput();
	// Returns true if terminating
	bool Update();
	void StartFrame();
	void SubmitFrame();

#if DEBUGGING
	void ModifyTickRate(Time newTickRate);
#endif

private:
	void PollEvents();
};
} // namespace LittleEngine
