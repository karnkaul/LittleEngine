#include "stdafx.h"
#include "ShapeRenderer.h"
#include "Engine/Logger/Logger.h"
#include "SFMLInterface/WindowController.h"
#include "RenderParams.h"

namespace Game {
	void ShapeRenderer::Render(RenderParams & params) {
		SetPosition(params.screenPosition);
		SetRotation(params.screenRotation);
		params.GetWindowController().Draw(*shape);
	}

	void ShapeRenderer::SetFillColour(const Colour & colour) {
		shape->setFillColor(Convert(colour));
	}

	void ShapeRenderer::SetBorder(Fixed width, const Colour & colour) {
		shape->setOutlineColor(Convert(colour));
		if (width < 0) {
			width = 0;
		}
		shape->setOutlineThickness(width.GetFloat());
	}

	void ShapeRenderer::SetPosition(const Vector2 screenPosition) {
		shape->setPosition(Convert(screenPosition));
	}

	void ShapeRenderer::SetRotation(const Fixed screenRotation) {
		shape->setRotation(screenRotation.GetFloat());
	}

	CircleRenderer::CircleRenderer(Fixed radius)
		: ShapeRenderer("CircleRenderer", std::make_unique<sf::CircleShape>(radius.GetFloat())) {
		circle = &CastShape<sf::CircleShape>();
		circle->setOrigin(radius.GetFloat(), radius.GetFloat());
	}

	CircleRenderer::CircleRenderer(Fixed radius, const Colour& colour)
		: ShapeRenderer("CircleRenderer", std::make_unique<sf::CircleShape>(radius.GetFloat())) {
		circle = &CastShape<sf::CircleShape>();
		circle->setOrigin(radius.GetFloat(), radius.GetFloat());
		SetFillColour(colour);
	}

	Rect2 CircleRenderer::GetBounds() const {
		Fixed radius(circle->getRadius());
		return Rect2(
			Vector2(-radius, -radius),
			Vector2(radius, radius)
		);
	}

	RectangleRenderer::RectangleRenderer(Vector2 size)
		: ShapeRenderer("RectangleRenderer", std::make_unique<sf::RectangleShape>(sf::Vector2f(size.x.GetFloat(), size.y.GetFloat()))) {
		rectangle = &CastShape<sf::RectangleShape>();
		rectangle->setOrigin(rectangle->getSize().x * 0.5f, rectangle->getSize().y * 0.5f);
	}

	RectangleRenderer::RectangleRenderer(Vector2 size, Colour colour)
		: ShapeRenderer("RectangleRenderer", std::make_unique<sf::RectangleShape>(sf::Vector2f(size.x.GetFloat(), size.y.GetFloat()))) {
		rectangle = &CastShape<sf::RectangleShape>();
		rectangle->setOrigin(rectangle->getSize().x * 0.5f, rectangle->getSize().y * 0.5f);
		SetFillColour(colour);
	}

	Rect2 RectangleRenderer::GetBounds() const {
		sf::Vector2f size = rectangle->getSize();
		Fixed width(size.x);
		Fixed height(size.y);
		return Rect2(
			Vector2(-width * Fixed::Half, -height * Fixed::Half),
			Vector2(width * Fixed::Half, height * Fixed::Half)
		);
	}
}
