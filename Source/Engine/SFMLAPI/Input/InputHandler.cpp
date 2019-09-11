#include "SFML/Window/Event.hpp"
#include "Core/Logger.h"
#include "InputHandler.h"
#include "SFMLAPI/Viewport/Viewport.h"

namespace LittleEngine
{
InputHandler::InputHandler() = default;
InputHandler::~InputHandler() = default;

ViewportEventType InputHandler::PollEvents(Viewport& sfWindow)
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
			LOG_D("[InputHandler] Window Closed requested");
			return ViewportEventType::Closed;
		}

		case sf::Event::LostFocus:
		{
			LOG_D("[InputHandler] Window Lost Focus");
			m_inputSM.ResetKeyStates();
			return ViewportEventType::LostFocus;
		}

		case sf::Event::GainedFocus:
		{
			LOG_D("[InputHandler] Window Gained Focus");
			m_inputSM.ResetKeyStates();
			return ViewportEventType::GainedFocus;
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
			m_inputSM.OnMouseMove(sfWindow.ViewportToWorld(sfEvent.mouseMove.x, sfEvent.mouseMove.y));
			break;
		}

		// Catch all
		default:
			return ViewportEventType::None;
			;
		}
	}
	m_inputSM.SetMouseWheelScroll(mouseWheelScroll);
	return ViewportEventType::None;
}

InputDataFrame InputHandler::Dataframe() const
{
	return m_inputSM.Dataframe();
}
} // namespace LittleEngine
