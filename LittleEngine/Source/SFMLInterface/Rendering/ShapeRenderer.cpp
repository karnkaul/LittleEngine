#include "le_stdafx.h"
#include "ShapeRenderer.h"
#include "Engine/Logger/Logger.h"
#include "SFMLInterface/WindowController.h"
#include "RenderParams.h"

namespace LittleEngine {
	void ShapeRenderer::RenderInternal(RenderParams & params) {
		SetPosition(params.screenPosition);
		SetRotation(params.screenRotation);
		// Push shape into appropriate drawing layer
		params.GetWindowController().Push(Drawable(*shape, layer));
	}

	void ShapeRenderer::SetFillColour(const Colour & colour) {
		shape->setFillColor(Convert(colour));
	}

	void ShapeRenderer::SetBorder(const Fixed& width, const Colour & colour) {
		shape->setOutlineColor(Convert(colour));
		shape->setOutlineThickness(width > 0 ? width.GetFloat() : 0);
	}

	void ShapeRenderer::SetPosition(const Vector2& screenPosition) {
		shape->setPosition(Convert(screenPosition));
	}

	void ShapeRenderer::SetRotation(const Fixed& screenRotation) {
		shape->setRotation(screenRotation.GetFloat());
	}

	CircleRenderer::CircleRenderer(const Fixed& radius) : ShapeRenderer("CircleRenderer", std::make_unique<sf::CircleShape>(radius.GetFloat())) {
		circle = &CastShape<sf::CircleShape>();
		circle->setOrigin(radius.GetFloat(), radius.GetFloat());
	}

	CircleRenderer::CircleRenderer(const Fixed& radius, const Colour& colour) : ShapeRenderer("CircleRenderer", std::make_unique<sf::CircleShape>(radius.GetFloat())) {
		circle = &CastShape<sf::CircleShape>();
		circle->setOrigin(radius.GetFloat(), radius.GetFloat());
		SetFillColour(colour);
	}

	CircleRenderer::CircleRenderer(const CircleRenderer & prototype) : ShapeRenderer(prototype.name, std::make_unique<sf::CircleShape>()) {
		circle = &CastShape<sf::CircleShape>();
		circle->setOrigin(prototype.circle->getOrigin());
		circle->setRadius(prototype.circle->getRadius());
		circle->setFillColor(prototype.circle->getFillColor());
	}

	void CircleRenderer::SetRadius(const Fixed& radius) {
		circle->setRadius(radius.GetFloat());
		// Reposition local centre
		circle->setOrigin(radius.GetFloat(), radius.GetFloat());
	}

	Rect2 CircleRenderer::GetBounds() const {
		Fixed radius(circle->getRadius());
		return Rect2(
			Vector2(-radius, -radius),
			Vector2(radius, radius)
		);
	}

	std::unique_ptr<Renderer> CircleRenderer::UClone() const {
		return std::make_unique<CircleRenderer>(*this);
	}

	RectangleRenderer::RectangleRenderer(const Vector2& size) : ShapeRenderer("RectangleRenderer", std::make_unique<sf::RectangleShape>(sf::Vector2f(size.x.GetFloat(), size.y.GetFloat()))) {
		rectangle = &CastShape<sf::RectangleShape>();
		rectangle->setOrigin(rectangle->getSize().x * 0.5f, rectangle->getSize().y * 0.5f);
	}

	RectangleRenderer::RectangleRenderer(const Vector2& size, const Colour& colour) : ShapeRenderer("RectangleRenderer", std::make_unique<sf::RectangleShape>(sf::Vector2f(size.x.GetFloat(), size.y.GetFloat()))) {
		rectangle = &CastShape<sf::RectangleShape>();
		rectangle->setOrigin(rectangle->getSize().x * 0.5f, rectangle->getSize().y * 0.5f);
		SetFillColour(colour);
	}

	RectangleRenderer::RectangleRenderer(const RectangleRenderer & prototype) : ShapeRenderer(prototype.name, std::make_unique<sf::RectangleShape>()) {
		rectangle = &CastShape<sf::RectangleShape>();
		rectangle->setOrigin(prototype.rectangle->getOrigin());
		rectangle->setSize(prototype.rectangle->getSize());
		rectangle->setFillColor(prototype.rectangle->getFillColor());
	}

	void RectangleRenderer::SetSize(const Vector2& size) {
		rectangle->setSize(Convert(size));
		// Reposition local centre
		rectangle->setOrigin(rectangle->getSize().x * 0.5f, rectangle->getSize().y * 0.5f);
	}

	Rect2 RectangleRenderer::GetBounds() const {
		sf::Vector2f size = rectangle->getSize();
		Fixed width = Fixed(size.x) * Fixed::OneHalf;
		Fixed height = Fixed(size.y) * Fixed::OneHalf;
		return Rect2(
			Vector2(-width, -height),
			Vector2(width, height)
		);
	}
	std::unique_ptr<Renderer> RectangleRenderer::UClone() const {
		return std::make_unique<RectangleRenderer>(*this);
	}
}
