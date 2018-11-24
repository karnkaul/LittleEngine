#include "le_stdafx.h"
#include "ShapeRenderable.h"
#include "Engine/Logger/Logger.h"
#include "SFMLInterface/WindowController.h"
#include "RenderParams.h"

namespace LittleEngine {
	ShapeRenderable::ShapeRenderable(std::string name, std::unique_ptr<sf::Shape> shape) : Renderable(name), shape(std::move(shape)) {
	}

	ShapeRenderable::ShapeRenderable(const ShapeRenderable & prototype, std::unique_ptr<sf::Shape> shape) : Renderable(prototype), shape(std::move(shape)) {
	}

	void ShapeRenderable::RenderInternal(RenderParams & params) {
		SetPosition(params.screenPosition);
		SetRotation(params.screenRotation);
		SetScale(params.screenScale);
		// Push shape into appropriate drawing layer
		params.GetWindowController().Push(Drawable(*shape, layer));
	}

	void ShapeRenderable::SetFillColour(const Colour & colour) {
		shape->setFillColor(Convert(colour));
	}

	void ShapeRenderable::SetBorder(const Fixed& width, const Colour & colour) {
		shape->setOutlineColor(Convert(colour));
		shape->setOutlineThickness(width > 0 ? width.ToFloat() : 0);
	}

	void ShapeRenderable::SetPosition(const Vector2& screenPosition) {
		shape->setPosition(Convert(screenPosition));
	}

	void ShapeRenderable::SetRotation(const Fixed& screenRotation) {
		shape->setRotation(screenRotation.ToFloat());
	}

	void ShapeRenderable::SetScale(const Vector2 & screenScale) {
		shape->setScale(Convert(screenScale));
	}

	CircleRenderable::CircleRenderable(const Fixed& radius) : ShapeRenderable("CircleRenderable", std::make_unique<sf::CircleShape>(radius.ToFloat())) {
		circle = &CastShape<sf::CircleShape>();
		circle->setOrigin(radius.ToFloat(), radius.ToFloat());
	}

	CircleRenderable::CircleRenderable(const Fixed& radius, const Colour& colour) : ShapeRenderable("CircleRenderable", std::make_unique<sf::CircleShape>(radius.ToFloat())) {
		circle = &CastShape<sf::CircleShape>();
		circle->setOrigin(radius.ToFloat(), radius.ToFloat());
		SetFillColour(colour);
	}

	CircleRenderable::CircleRenderable(const CircleRenderable & prototype) : ShapeRenderable(prototype, std::make_unique<sf::CircleShape>()) {
		circle = &CastShape<sf::CircleShape>();
		circle->setOrigin(prototype.circle->getOrigin());
		circle->setRadius(prototype.circle->getRadius());
		circle->setFillColor(prototype.circle->getFillColor());
		circle->setOutlineColor(prototype.circle->getOutlineColor());
		circle->setOutlineThickness(prototype.circle->getOutlineThickness());
	}

	void CircleRenderable::SetRadius(const Fixed& radius) {
		circle->setRadius(radius.ToFloat());
		// Reposition local centre
		circle->setOrigin(radius.ToFloat(), radius.ToFloat());
	}

	Rect2 CircleRenderable::GetBounds() const {
		Fixed radius(circle->getRadius());
		return Rect2(
			Vector2(-radius, -radius),
			Vector2(radius, radius)
		);
	}

	std::unique_ptr<Renderable> CircleRenderable::UClone() const {
		return std::make_unique<CircleRenderable>(*this);
	}

	RectangleRenderable::RectangleRenderable(const Vector2& size) : ShapeRenderable("RectangleRenderable", std::make_unique<sf::RectangleShape>(sf::Vector2f(size.x.ToFloat(), size.y.ToFloat()))) {
		rectangle = &CastShape<sf::RectangleShape>();
		rectangle->setOrigin(rectangle->getSize().x * 0.5f, rectangle->getSize().y * 0.5f);
	}

	RectangleRenderable::RectangleRenderable(const Vector2& size, const Colour& colour) : ShapeRenderable("RectangleRenderable", std::make_unique<sf::RectangleShape>(sf::Vector2f(size.x.ToFloat(), size.y.ToFloat()))) {
		rectangle = &CastShape<sf::RectangleShape>();
		rectangle->setOrigin(rectangle->getSize().x * 0.5f, rectangle->getSize().y * 0.5f);
		SetFillColour(colour);
	}

	RectangleRenderable::RectangleRenderable(const RectangleRenderable & prototype) : ShapeRenderable(prototype, std::make_unique<sf::RectangleShape>()) {
		rectangle = &CastShape<sf::RectangleShape>();
		rectangle->setOrigin(prototype.rectangle->getOrigin());
		rectangle->setSize(prototype.rectangle->getSize());
		rectangle->setFillColor(prototype.rectangle->getFillColor());
		rectangle->setOutlineColor(prototype.rectangle->getOutlineColor());
		rectangle->setOutlineThickness(prototype.rectangle->getOutlineThickness());
	}

	void RectangleRenderable::SetSize(const Vector2& size) {
		rectangle->setSize(Convert(size));
		// Reposition local centre
		rectangle->setOrigin(rectangle->getSize().x * 0.5f, rectangle->getSize().y * 0.5f);
	}

	Rect2 RectangleRenderable::GetBounds() const {
		sf::Vector2f size = rectangle->getSize();
		Fixed width = Fixed(size.x) * Fixed::OneHalf;
		Fixed height = Fixed(size.y) * Fixed::OneHalf;
		return Rect2(
			Vector2(-width, -height),
			Vector2(width, height)
		);
	}

	std::unique_ptr<Renderable> RectangleRenderable::UClone() const {
		return std::make_unique<RectangleRenderable>(*this);
	}
}
