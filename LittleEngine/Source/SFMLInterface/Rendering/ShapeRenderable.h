#pragma once
#include "SFML/Graphics.hpp"
#include "Vector2.h"
#include "Renderable.h"

namespace LittleEngine {
	// \brief Abstract class for drawing SFML Shapes
	class ShapeRenderable : public Renderable {
	protected:
		std::unique_ptr<sf::Shape> m_uShape;	// sf::Shape cannot be a direct member

	public:
		void SetFillColour(const Colour& colour);
		void SetBorder(const Fixed& width, const Colour& colour);

		ShapeRenderable(std::string name, std::unique_ptr<sf::Shape> shape);
		ShapeRenderable(const ShapeRenderable& prototype, std::unique_ptr<sf::Shape> shape);

	protected:
		virtual void RenderInternal() override; 
		virtual void SetPosition(const Vector2& screenPosition) override;
		virtual void SetOrientation(const Fixed& screenOrientation) override;
		virtual void SetScale(const Vector2& screenScale) override;

		template<typename T>
		T& CastShape() const {
			static_assert(std::is_base_of<sf::Shape, T>::value, "T must derive from sf::Shape; check Output window for erroneous call");
			return dynamic_cast<T&>(*m_uShape);
		}
	};

	// \brief Concrete class to draw a CircleShape
	class CircleRenderable : public ShapeRenderable {
	private:
		sf::CircleShape* m_pCircle;

	public:
		CircleRenderable(const Fixed& radius);
		CircleRenderable(const Fixed& radius, const Colour& colour);
		CircleRenderable(const CircleRenderable& prototype);

		void SetRadius(const Fixed& radius);

		// Returns outer rect of CircleShape (|any point| = radius)
		virtual Rect2 GetBounds() const override;
		virtual std::unique_ptr<Renderable> UClone() const override;
		virtual void SetPivot(const Vector2& pivot) override;
	};

	// \brief Concrete class to draw a RectangleShape
	class RectangleRenderable : public ShapeRenderable {
	private:
		sf::RectangleShape* m_pRectangle;

	public:
		RectangleRenderable(const Vector2& size);
		RectangleRenderable(const Vector2& size, const Colour& colour);
		RectangleRenderable(const RectangleRenderable& prototype);

		void SetSize(const Vector2& size);

		// Returns bounds of RectangleShape
		virtual Rect2 GetBounds() const override;
		virtual std::unique_ptr<Renderable> UClone() const override;
		virtual void SetPivot(const Vector2& pivot) override;
	};
}

