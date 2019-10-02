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
	bool bRenderThread = true;
};

class LEContext final
{
public:
	using OnSubmit = Core::Delegate<>;
	using Pointer = sf::Cursor;

private:
	struct PtrEntry
	{
		WToken wToken;
		Pointer* pPointer = nullptr;
		Pointer::Type type;

		PtrEntry(WToken wToken, Pointer* pPointer, Pointer::Type type);
	};

private:
	LEContextData m_data;
	UMap<Pointer::Type, UPtr<Pointer>> m_pointerMap;
	Vec<PtrEntry> m_pointerStack;
	Token m_ptrToken;
	OnSubmit m_onSubmitted;
	UPtr<class LEInput> m_uInput;
	UPtr<class Viewport> m_uViewport;
	UPtr<class LERenderer> m_uRenderer;
#if ENABLED(DEBUG_LOGGING)
	Pointer::Type m_prevPtrType;
#endif
	bool m_bTerminating = false;
	bool m_bWaitingToTerminate = false;

	std::optional<ViewportStyle> m_oNewViewportStyle;
	std::optional<const ViewportSize*> m_oNewViewportSize;

public:
	LEContext(LEContextData data);
	~LEContext();

	bool IsTerminating() const;

	LEInput* Input() const;
	LERenderer* Renderer() const;

	Time MaxFrameTime() const;

	bool TrySetViewportSize(u32 height);
	void SetWindowStyle(ViewportStyle newStyle);
	void Terminate();

	void PollInput();
	void FireInput();
	void Update();
	void StartFrame();
	void SubmitFrame();

	Token PushPointer(Pointer::Type type);
	Token RegisterOnSubmitted(OnSubmit::Callback onFrameSubmitted);

#if defined(DEBUGGING)
	void ModifyTickRate(Time newTickRate);
#endif

private:
	Pointer* GetPointer(Pointer::Type type);
};
} // namespace LittleEngine
