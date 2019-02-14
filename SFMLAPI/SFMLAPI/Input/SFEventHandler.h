#pragma once
#include "SFInputStateMachine.h"

namespace LittleEngine
{
enum class SFWindowEventType
{
	None,
	Closed,
	LostFocus,
	GainedFocus,
	Resized
};

class SFEventHandler
{
protected:
	SFInputStateMachine m_inputSM;

public:
	SFWindowEventType PollEvents(class SFWindow& sfWindow);
	SFInputDataFrame GetFrameInputData() const;
};
} // namespace LittleEngine
