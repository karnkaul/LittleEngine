#pragma once
#include "SFML/Graphics.hpp"
#include "Utils/Fixed.h"
#include "Input.h"
#include "SystemClock.h"
#include "Rendering/RenderParams.h"
#include <array>

namespace Game {
	// Wrapper struct for SFML Drawable
	struct Drawable {
	public:
		LayerInfo layer;

		Drawable(sf::Drawable& drawable) : drawable(drawable) { }
		Drawable(sf::Drawable& drawable, LayerInfo layer) : drawable(drawable), layer(layer) {}
		Drawable(Drawable&&) = default;
		Drawable& operator=(Drawable&&) = default;
		Drawable(const Drawable&) = default;

		const sf::Drawable& GetSFMLDrawable() { return drawable; }

	private:
		sf::Drawable& drawable;
	};

	// Conrete class that can create an SFML RenderWindow,
	// and draw a buffer of Drawables to it.
	// Provides an InputHandler to poll inputs every frame
	class WindowController {
	public:
		static constexpr int MAX_LAYERS = 100;
		static constexpr int MAX_LAYERID = MAX_LAYERS - 1;

		WindowController(int screenWidth, int screenHeight, std::string windowTitle);
		~WindowController();
		// Convenience methods for Game Loop etc
		bool IsWindowOpen() const;
		bool IsWindowFocussed() const;

		// Call this to update InputHandler's state for this frame
		void PollInput();
		// Add drawable to buffer
		void Push(Drawable drawable);
		// Clear screen and draw current buffer
		void Draw();
		// Destroy SFML RenderWindow
		void CloseWindow();
		const Input& GetInputHandler() const;

	private:
		std::array<std::vector<Drawable>, MAX_LAYERS> buffer;
		WindowController(const WindowController&) = delete;
		WindowController& operator=(const WindowController&) = delete;
		std::unique_ptr<sf::RenderWindow> window;
		Input input;
		bool _focus = false;
	};
}
