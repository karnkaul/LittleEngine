#pragma once
#include "SFML/Graphics.hpp"
#include "Fixed.h"
#include "Input.h"
#include "Rect2.h"
#include "SystemClock.h"
#include "Rendering/RenderParams.h"
#include <array>
#include <functional>

namespace LittleEngine {
	using Vector2 = GameUtils::Vector2;
	using Rect2 = GameUtils::Rect2;

	struct GraphicsData {
		std::string windowTitle = "Untitled";
		Vector2 viewSize = Vector2(1920, 1080);
		unsigned int screenWidth = 1280;
		unsigned int screenHeight = 720;
	};

	// \brief Wrapper struct for SFML Drawable
	struct Drawable {
	private:
		sf::Drawable* drawable;
	public:
		LayerInfo layer;
	
		Drawable(sf::Drawable& drawable) : drawable(&drawable) { }
		Drawable(sf::Drawable& drawable, LayerInfo layer) : drawable(&drawable), layer(layer) {}
		Drawable(Drawable&&) = default;
		Drawable& operator=(Drawable&&) = default;
		Drawable(const Drawable&) = default;

		const sf::Drawable& GetSFMLDrawable() { return *drawable; }
	};

	struct GraphicsException {
		std::string errMsg;
		
		GraphicsException(const std::string& errMsg) : errMsg(errMsg) {}
	};

	// \brief Conrete class that can create an SFML RenderWindow,
	// and draw a buffer of Drawables to it.
	// Provides an InputHandler to poll inputs every frame
	class Graphics final {
	private:
		// \brief Sprite buffer; contains MAX_LAYERS layers, 
		// each with a vector of Drawables.
		struct Buffer {
			static constexpr int MAX_LAYERS = 100;

			void Push(Drawable&& drawable, int index);
			void ForEach(std::function<void(std::vector<Drawable>&)> Callback);
			void Clear();

		private:
			std::array<std::vector<Drawable>, MAX_LAYERS> buffer;
		};
	
	public:
		static constexpr int MAX_LAYERID = Buffer::MAX_LAYERS - 1;

		static sf::Vector2f Cast(const Vector2& vector);
		static Vector2 Cast(const sf::Vector2f& vector);
		static sf::Color Cast(const Colour& colour);
		static Colour Cast(const sf::Color& colour);

		static Vector2 GetGameViewSize();
		static Rect2 GetWorldBounds();
		static Vector2 NormalisedToWorldPoint(const Vector2& normalised, bool autoClamp = true);// Add drawable to buffer
		static void Submit(Drawable&& drawable);

	private:
		static std::unique_ptr<Graphics> m_instance;

		static Graphics& CreateInstance(const GraphicsData& data);
		static bool DestroyInstance();
		
	private:
		Buffer m_buffer;
		Input m_input;
		Vector2 m_gameViewSize;
		Rect2 m_worldBounds;
		std::unique_ptr<sf::RenderWindow> m_uWindow;
		bool m_bFocus = false;

	public:
		Graphics(const GraphicsData& data);
		~Graphics();

		// For Game Loop
		bool IsWindowOpen() const;
		// For Game Loop
		bool IsWindowFocussed() const;

		// Call this to update InputHandler's state for this frame
		void PollEvents();
		// Add drawable to buffer
		void Push(Drawable&& drawable);
		// Clear screen and draw current buffer
		void Draw();
		// Destroy SFML RenderWindow
		void CloseWindow();
		const Input& GetInput() const;

	private:
		sf::View gameView;

		Graphics(const Graphics&) = delete;
		Graphics& operator=(const Graphics&) = delete;	

		friend class Engine;
	};
}
