#include "stdafx.h"
#include "SFEventHandler.h"
#include "SFMLAPI/Windowing/SFWindow.h"
#include "Logger.h"

namespace LittleEngine
{
SFWindowEventType SFEventHandler::PollEvents(SFWindow& sfWindow)
{
	sf::Event sfEvent;
	m_inputSM.ClearTextInput();
	MouseInput pointerInput;
	pointerInput.worldPosition = sfWindow.ScreenToWorld(sf::Mouse::getPosition(sfWindow));
	pointerInput.bLeftPressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	pointerInput.bRightPressed = sf::Mouse::isButtonPressed(sf::Mouse::Right);
	m_inputSM.SetPointerState(pointerInput);
	while (sfWindow.pollEvent(sfEvent))
	{
		switch (sfEvent.type)
		{
		// Window
		case sf::Event::Closed:
		{
			LOG_D("[SFEventHandler] Window Closed requested");
			return SFWindowEventType::Closed;
		}

		case sf::Event::LostFocus:
		{
			LOG_D("[SFEventHandler] Window Lost Focus");
			m_inputSM.ResetKeyStates();
			return SFWindowEventType::LostFocus;
		}

		case sf::Event::GainedFocus:
		{
			LOG_D("[SFEventHandler] Window Gained Focus");
			m_inputSM.ResetKeyStates();
			return SFWindowEventType::GainedFocus;
		}

		case sf::Event::KeyPressed:
		{
			m_inputSM.OnKeyDown(sfEvent.key);
			break;
		}

		// Input
		case sf::Event::KeyReleased:
		{
			m_inputSM.OnKeyUp(sfEvent.key);
			break;
		}

		case sf::Event::TextEntered:
		{
			m_inputSM.OnTextInput(sfEvent.text.unicode);
			break;
		}

		// Catch all
		default:
			return SFWindowEventType::None;
			;
		}
	}
	return SFWindowEventType::None;
}

SFInputDataFrame SFEventHandler::GetFrameInputData() const
{
	return m_inputSM.GetFrameInputData();
}
} // namespace LittleEngine
