#pragma once
#include "SFML/Graphics.hpp"
#include "Utils/Fixed.h"
#include "Input.h"
#include "SystemClock.h"

namespace Game {
	// Wrapper struct for SFML Drawable
	struct Drawable {
	public:
		Drawable(sf::Drawable& drawable) : drawable(&drawable) { }
		const sf::Drawable& GetSFMLDrawable() { return *drawable; }
	private:
		sf::Drawable* drawable;
	};

	// Conrete class that can create an SFML RenderWindow 
	// Provides an InputHandler to poll inputs every frame
	class WindowController {
	public:
		WindowController(int screenWidth, int screenHeight, std::string windowTitle);
		~WindowController();
		// Convenience methods for Game Loop etc
		bool IsWindowOpen() const;
		bool IsWindowFocussed() const;

		// Call this to update InputHandler's state for this frame
		void PollInput();
		// Clear the window of all drawing
		void Clear();
		// Draw an entity
		void Draw(Drawable drawable);
		// Call SFML RenderWindow::Display()
		void Display();
		// Destroy SFML RenderWindow
		void CloseWindow();
		const InputHandler& GetInputHandler() const;
	private:
		WindowController(const WindowController&) = delete;
		WindowController& operator=(const WindowController&) = delete;
		std::unique_ptr<sf::RenderWindow> window;
		InputHandler inputHandler;
		bool _focus = false;
	};
}
