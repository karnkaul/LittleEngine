#include "stdafx.h"
#include <string>
#include "Engine/Logger/Logger.h"
#include "WindowController.h"

namespace Game {
	WindowController::WindowController(int screenWidth, int screenHeight, std::string windowTitle) {
		window = std::make_unique<sf::RenderWindow>(sf::VideoMode(
			screenWidth,
			screenHeight),
			windowTitle
			);
		_focus = true;
	}

	WindowController::~WindowController() {
		Logger::Log("WindowController and RenderWindow destroyed");
	}

	bool WindowController::IsWindowOpen() const {
		return window->isOpen();
	}

	bool WindowController::IsWindowFocussed() const {
		return _focus;
	}

	void WindowController::PollInput() {
		sf::Event event;
		while (window->isOpen() && window->pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				CloseWindow();
				break;

			case sf::Event::LostFocus:
				_focus = false;
				break;

			case sf::Event::GainedFocus:
				_focus = true;
				inputHandler.ResetKeyStates();
				break;

			case sf::Event::KeyPressed:
				inputHandler.OnKeyDown(event.key);
				break;

			case sf::Event::KeyReleased:
				inputHandler.OnKeyUp(event.key);
			}
		}
	}

	void WindowController::Clear() {
		window->clear();
	}

	void WindowController::Draw(Drawable drawable) {
		window->draw(drawable.GetSFMLDrawable());
	}

	void WindowController::Display() {
		window->display();
	}

	void WindowController::CloseWindow() {
		window->close();
	}

	const Input& WindowController::GetInputHandler() const {
		return inputHandler;
	}
}
