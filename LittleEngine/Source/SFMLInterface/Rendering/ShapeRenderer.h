#pragma once
#include "SFML/Graphics.hpp"
#include "Utils/Vector2.h"
#include "Renderer.h"

namespace LittleEngine {
	// \brief Abstract class for drawing SFML Shapes
	class ShapeRenderer : public Renderer {
	public:
		void SetFillColour(const Colour& colour);
		void SetBorder(const Fixed& width, const Colour& colour);

	protected:
		std::unique_ptr<sf::Shape> shape;	// sf::Shape cannot be a direct member

		ShapeRenderer(std::string name, std::unique_ptr<sf::Shape> shape) : Renderer(name), shape(std::move(shape)) 
		{}

		virtual void RenderInternal(RenderParams& params) override; 
		virtual void SetPosition(const Vector2& screenPosition) override;
		virtual void SetRotation(const Fixed& screenRotation) override;

		template<typename T>
		T& CastShape() const {
			static_assert(std::is_base_of<sf::Shape, T>::value, "T must derive from sf::Shape; check Output window for erroneous call");
			return dynamic_cast<T&>(*shape);
		}
	};

	// \brief Concrete class to draw a CircleShape
	class CircleRenderer : public ShapeRenderer {
	public:
		CircleRenderer(const Fixed& radius);
		CircleRenderer(const Fixed& radius, const Colour& colour);

		void SetRadius(const Fixed& radius);

		// Returns outer rect of CircleShape (|any point| = radius)
		virtual Rect2 GetBounds() const override;
	private:
		sf::CircleShape* circle;
	};

	// \brief Concrete class to draw a RectangleShape
	class RectangleRenderer : public ShapeRenderer {
	public:
		RectangleRenderer(const Vector2& size);
		RectangleRenderer(const Vector2& size, const Colour& colour);

		void SetSize(const Vector2& size);

		// Returns bounds of RectangleShape
		virtual Rect2 GetBounds() const override;
	private:
		sf::RectangleShape* rectangle;
	};
}
