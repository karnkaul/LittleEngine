#include "stdafx.h"
#include "Core/Logger.h"
#include "SFInputHandler.h"
#include "SFMLAPI/Viewport/SFViewport.h"

namespace LittleEngine
{
SFInputHandler::SFInputHandler() = default;
SFInputHandler::~SFInputHandler() = default;

SFViewportEventType SFInputHandler::PollEvents(SFViewport& sfWindow)
{
	sf::Event sfEvent;
	m_inputSM.ClearTextInput();
	m_inputSM.UpdateJoyInput();
	Fixed mouseWheelScroll = Fixed::Zero;
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

		// Input
		case sf::Event::KeyPressed:
		{
			m_inputSM.OnKeyDown(sfEvent.key);
			break;
		}

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

		case sf::Event::MouseEntered:
		{
			m_inputSM.OnMouseChange(true);
			break;
		}

		case sf::Event::MouseLeft:
		{
			m_inputSM.OnMouseChange(false);
			break;
		}

		case sf::Event::MouseButtonPressed:
		{
			m_inputSM.OnMouseDown(sfEvent.mouseButton);
			break;
		}

		case sf::Event::MouseButtonReleased:
		{
			m_inputSM.OnMouseUp(sfEvent.mouseButton);
			break;
		}

		case sf::Event::MouseWheelScrolled:
		{
			mouseWheelScroll = Fixed(sfEvent.mouseWheelScroll.delta);
			break;
		}

		case sf::Event::MouseMoved:
		{
			m_inputSM.OnMouseMove(sfWindow.ScreenToWorld(sfEvent.mouseMove.x, sfEvent.mouseMove.y));
			break;
		}

		// Catch all
		default:
			return SFViewportEventType::None;
			;
		}
	}
	m_inputSM.SetMouseWheelScroll(mouseWheelScroll);
	return SFViewportEventType::None;
}

SFInputDataFrame SFInputHandler::GetFrameInputData() const
{
	return m_inputSM.GetFrameInputData();
}
} // namespace LittleEngine
