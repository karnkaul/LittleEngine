#pragma once
#include "SFML/Graphics.hpp"
#include "Utils/Vector2.h"
#include "Renderer.h"

namespace Game {
	// Abstract class for drawing SFML Shapes
	class ShapeRenderer : public Renderer {
	public:
		virtual void Render(RenderParams& params) override;

	protected:
		std::unique_ptr<sf::Shape> shape;	// sf::Shape cannot be a direct member

		ShapeRenderer(std::string name, std::unique_ptr<sf::Shape> shape) : Renderer(name) {
			this->shape = std::move(shape);
		}

		template<typename T>
		T& CastShape() const {
			static_assert(std::is_base_of<sf::Shape, T>::value, "T must derive from sf::Shape; check Output window for erroneous call");
			return dynamic_cast<T&>(*shape);
		}
	};

	// Concrete class to draw a CircleShape
	class CircleRenderer : public ShapeRenderer {
	public:
		CircleRenderer(Fixed radius);
		CircleRenderer(Fixed radius, const Colour& colour);
		virtual void SetPosition(const Vector2 screenPosition) override;
		virtual void SetRotation(const Fixed screenRotation) override;
		// Returns radius of CircleShape
		virtual Vector2 GetBounds() const override;
	};

	// Concrete class to draw a RectangleShape
	class RectangleRenderer : public ShapeRenderer {
	public:
		RectangleRenderer(Vector2 size);
		RectangleRenderer(Vector2 size, Colour colour);
		virtual void SetPosition(const Vector2 screenPosition) override;
		virtual void SetRotation(const Fixed screenRotation) override;
		// Returns bounds of RectangleShape
		virtual Vector2 GetBounds() const override;
	};
}
