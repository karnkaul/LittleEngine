#pragma once
#include "SFML/Window/Cursor.hpp"
#include "Core/CoreTypes.h"
#include "Core/Delegate.h"
#include "SFMLAPI/Input/InputMappings.h"
#include "SFMLAPI/Viewport/ViewportData.h"

namespace LittleEngine
{
struct LEContextData
{
	ViewportData viewportData;
	InputMap inputMap;
	Time tickRate = Time::Seconds(1.0f / 60.0f);
	Time maxFrameTime = Time::Milliseconds(50);
	Time renderThreadStartDelay = Time::Milliseconds(10);
	bool bPauseOnFocusLoss = false;
	bool bRenderThread = true;
};

class LEContext final
{
public:
	using OnSubmit = Core::Delegate<>;
	using Token = SPtr<s32>;
	using Pointer = sf::Cursor;

private:
	struct PtrEntry
	{
		WPtr<s32> wToken;
		UPtr<Pointer> uPointer;
	};

private:
	LEContextData m_data;
	List<PtrEntry> m_pointerStack;
	Token m_ptrToken;
	OnSubmit m_onSubmitted;
	UPtr<class LEInput> m_uInput;
	UPtr<class Viewport> m_uViewport;
	UPtr<class LERenderer> m_uRenderer;
	bool m_bTerminating = false;
	bool m_bWaitingToTerminate = false;
	bool m_bPauseTicking = false;

	std::optional<ViewportStyle> m_oNewViewportStyle;
	std::optional<const ViewportSize*> m_oNewViewportSize;

public:
	LEContext(LEContextData data);
	~LEContext();

	bool IsTerminating() const;
	bool IsPaused() const;

	LEInput* Input() const;
	LERenderer* Renderer() const;

	Time MaxFrameTime() const;

	bool TrySetViewportSize(u32 height);
	void SetWindowStyle(ViewportStyle newStyle);
	Token PushPointer(Pointer::Type type);
	void Terminate();

	void PollInput();
	void FireInput();
	// Returns true if terminating
	bool Update();
	void StartFrame();
	void SubmitFrame();

	OnSubmit::Token RegisterOnSubmitted(OnSubmit::Callback onFrameSubmitted);

#if defined(DEBUGGING)
	void ModifyTickRate(Time newTickRate);
#endif
};
} // namespace LittleEngine
