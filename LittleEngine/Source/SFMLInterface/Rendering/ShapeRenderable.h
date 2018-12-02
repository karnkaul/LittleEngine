#pragma once
#include "SFML/Graphics.hpp"
#include "Vector2.h"
#include "Renderable.h"

namespace LittleEngine {
	// \brief Abstract class for drawing SFML Shapes
	class ShapeRenderable : public Renderable {
	public:
		void SetFillColour(const Colour& colour);
		void SetBorder(const Fixed& width, const Colour& colour);

	protected:
		std::unique_ptr<sf::Shape> shape;	// sf::Shape cannot be a direct member

		ShapeRenderable(std::string name, std::unique_ptr<sf::Shape> shape);
		ShapeRenderable(const ShapeRenderable& prototype, std::unique_ptr<sf::Shape> shape);

		virtual void RenderInternal(RenderParams& params) override; 
		virtual void SetPosition(const Vector2& screenPosition) override;
		virtual void SetRotation(const Fixed& screenRotation) override;
		virtual void SetScale(const Vector2& screenScale) override;

		template<typename T>
		T& CastShape() const {
			static_assert(std::is_base_of<sf::Shape, T>::value, "T must derive from sf::Shape; check Output window for erroneous call");
			return dynamic_cast<T&>(*shape);
		}
	};

	// \brief Concrete class to draw a CircleShape
	class CircleRenderable : public ShapeRenderable {
	public:
		CircleRenderable(const Fixed& radius);
		CircleRenderable(const Fixed& radius, const Colour& colour);
		CircleRenderable(const CircleRenderable& prototype);

		void SetRadius(const Fixed& radius);

		// Returns outer rect of CircleShape (|any point| = radius)
		virtual Rect2 GetBounds() const override;
		virtual std::unique_ptr<Renderable> UClone() const override;

	private:
		sf::CircleShape* circle;
	};

	// \brief Concrete class to draw a RectangleShape
	class RectangleRenderable : public ShapeRenderable {
	public:
		RectangleRenderable(const Vector2& size);
		RectangleRenderable(const Vector2& size, const Colour& colour);
		RectangleRenderable(const RectangleRenderable& prototype);

		void SetSize(const Vector2& size);

		// Returns bounds of RectangleShape
		virtual Rect2 GetBounds() const override;
		virtual std::unique_ptr<Renderable> UClone() const override;

	private:
		sf::RectangleShape* rectangle;
	};
}

