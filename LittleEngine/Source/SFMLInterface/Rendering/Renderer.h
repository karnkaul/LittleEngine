#pragma once
#include "Utils/Vector2.h"
#include "Engine/Object.h"
#include "Utils/Rect2.h"
#include "RenderParams.h"
#include "SFML/Graphics.hpp"

namespace Game {
	// Wrapper class for SFML Color
	struct Colour {
	public:
		Fixed r;
		Fixed g;
		Fixed b;
		Fixed a;

		Colour(Fixed r = 255, Fixed g = 255, Fixed b = 255, Fixed a = 255) : r(r), g(g), b(b), a(a) {}
		
		const static Colour Black;
		const static Colour White;
		const static Colour Red;
		const static Colour Green;
		const static Colour Blue;
		const static Colour Yellow;
		const static Colour Magenta;
		const static Colour Cyan;
		const static Colour Transparent;
	};

	// Base class for all SFML Rendering
	class Renderer : public Object {
	public:
		LayerInfo layer;

		Renderer(std::string name);
		virtual ~Renderer();
		// Call this to render the entity using the passed RenderParams
		void Render(struct RenderParams& params);
		// Subclass will return its max Bounds in screen space
		virtual Rect2 GetBounds() const = 0;
		
		bool IsEnabled() const;
		void SetEnabled(bool enabled);
	
	protected:
		virtual void RenderInternal(struct RenderParams& params) = 0;
		virtual void SetPosition(const Vector2 screenPosition) = 0;
		virtual void SetRotation(const Fixed screenRotation) = 0;
		sf::Vector2f Convert(const Vector2& vector);
		sf::Color Convert(const Colour& colour);
		Colour Convert(const sf::Color& colour);

	private:
		bool isEnabled = true;

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
	};
}
