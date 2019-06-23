#pragma once
#include "InputStateMachine.h"

namespace LittleEngine
{
enum class ViewportEventType
{
	None,
	Closed,
	LostFocus,
	GainedFocus,
	Resized
};

class InputHandler
{
protected:
	InputStateMachine m_inputSM;

public:
	InputHandler();
	~InputHandler();

public:
	ViewportEventType PollEvents(class Viewport& sfWindow);
	InputDataFrame GetFrameInputData() const;
};
} // namespace LittleEngine
