#pragma once
#include "SFInputStateMachine.h"

namespace LittleEngine
{
enum class SFViewportEventType
{
	None,
	Closed,
	LostFocus,
	GainedFocus,
	Resized
};

class SFInputHandler
{
protected:
	SFInputStateMachine m_inputSM;

public:
	SFInputHandler();
	~SFInputHandler();

public:
	SFViewportEventType PollEvents(class SFViewport& sfWindow);
	SFInputDataFrame GetFrameInputData() const;
};
} // namespace LittleEngine
