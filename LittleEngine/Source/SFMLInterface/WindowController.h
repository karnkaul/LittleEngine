#pragma once
#include "SFML/Graphics.hpp"
#include "Utils/Fixed.h"
#include "Input.h"
#include "SystemClock.h"
#include "Rendering/RenderParams.h"
#include <array>
#include <functional>

namespace LittleEngine {
	// \brief Wrapper struct for SFML Drawable
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

	enum class RawTextInputType {
		None,
		Tab,
		Enter,
		Backspace,
		Escape,
	};

	struct RawTextInput {
		std::string text;
		RawTextInputType special;
	};

	// \brief Conrete class that can create an SFML RenderWindow,
	// and draw a buffer of Drawables to it.
	// Provides an InputHandler to poll inputs every frame
	class WindowController final {
	private:
		// \brief Sprite buffer; contains MAX_LAYERS layers, 
		// each with a vector of Drawables.
		struct Buffer {
			static constexpr int MAX_LAYERS = 100;

			void Push(Drawable&& drawable, int index);
			void ForEach(std::function<void(std::vector<Drawable>)> Callback) const;
			void Clear();

		private:
			std::array<std::vector<Drawable>, MAX_LAYERS> buffer;
		};

	public:
		static constexpr int MAX_LAYERID = Buffer::MAX_LAYERS - 1;

		WindowController(int screenWidth, int screenHeight, const std::string& windowTitle);
		~WindowController();

		// For Game Loop
		bool IsWindowOpen() const;
		// For Game Loop
		bool IsWindowFocussed() const;

		// Call this to update InputHandler's state for this frame
		void PollInput();
		// Add drawable to buffer
		void Push(Drawable&& drawable);
		// Clear screen and draw current buffer
		void Draw();
		// Destroy SFML RenderWindow
		void CloseWindow();
		const Input& GetInput() const;
		const RawTextInput& GetRawSFMLInput() const;

	private:
		Buffer buffer;
		Input input;
		std::unique_ptr<sf::RenderWindow> window;
		RawTextInput rawTextInput;
		bool _focus = false;

		WindowController(const WindowController&) = delete;
		WindowController& operator=(const WindowController&) = delete;	
	};
}
