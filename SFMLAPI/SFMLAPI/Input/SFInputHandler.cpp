#include "stdafx.h"
#include "SFInputHandler.h"
#include "SFMLAPI/Viewport/SFViewport.h"
#include "Core/Logger.h"

namespace LittleEngine
{
SFInputHandler::SFInputHandler() = default;
SFInputHandler::~SFInputHandler() = default;

SFViewportEventType SFInputHandler::PollEvents(SFViewport& sfWindow)
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
			LOG_D("[SFInputHandler] Window Closed requested");
			return SFViewportEventType::Closed;
		}

		case sf::Event::LostFocus:
		{
			LOG_D("[SFInputHandler] Window Lost Focus");
			m_inputSM.ResetKeyStates();
			return SFViewportEventType::LostFocus;
		}

		case sf::Event::GainedFocus:
		{
			LOG_D("[SFInputHandler] Window Gained Focus");
			m_inputSM.ResetKeyStates();
			return SFViewportEventType::GainedFocus;
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
			return SFViewportEventType::None;
			;
		}
	}
	return SFViewportEventType::None;
}

SFInputDataFrame SFInputHandler::GetFrameInputData() const
{
	return m_inputSM.GetFrameInputData();
}
} // namespace LittleEngine
