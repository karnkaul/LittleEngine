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

	CircleRenderer::CircleRenderer(Fixed radius)
		: ShapeRenderer("CircleRenderer", std::make_unique<sf::CircleShape>(radius.GetFloat())) {
		auto circle = CastShape<sf::CircleShape>();
		circle->setOrigin(radius.GetFloat(), radius.GetFloat());
	}

	CircleRenderer::CircleRenderer(Fixed radius, const Colour& colour)
		: ShapeRenderer("CircleRenderer", std::make_unique<sf::CircleShape>(radius.GetFloat())) {
		auto circle = CastShape<sf::CircleShape>();
		circle->setOrigin(radius.GetFloat(), radius.GetFloat());
		circle->setFillColor(Convert(colour));
	}

	void CircleRenderer::SetPosition(const Vector2 screenPosition) {
		auto circle = CastShape<sf::CircleShape>();
		circle->setPosition(Convert(screenPosition));
	}

	void CircleRenderer::SetRotation(const Fixed screenRotation) {
		auto circle = CastShape<sf::CircleShape>();
		circle->setRotation(screenRotation.GetFloat());
	}

	Vector2 CircleRenderer::GetBounds() const {
		auto circle = CastShape<sf::CircleShape>();
		return Vector2(
			static_cast<int>(circle->getRadius() * 2),
			static_cast<int>(circle->getRadius() * 2)
		);
	}

	RectangleRenderer::RectangleRenderer(Vector2 size)
		: ShapeRenderer("RectangleRenderer", std::make_unique<sf::RectangleShape>(Convert(size))) {
		auto rectangle = CastShape<sf::RectangleShape>();
		rectangle->setOrigin(rectangle->getSize().x * 0.5f, rectangle->getSize().y * 0.5f);
	}

	RectangleRenderer::RectangleRenderer(Vector2 size, Colour colour)
		: ShapeRenderer("RectangleRenderer", std::make_unique<sf::RectangleShape>(Convert(size))) {
		auto rectangle = CastShape<sf::RectangleShape>();
		rectangle->setOrigin(rectangle->getSize().x * 0.5f, rectangle->getSize().y * 0.5f);
		rectangle->setFillColor(Convert(colour));
	}

	void RectangleRenderer::SetPosition(const Vector2 screenPosition) {
		auto rectangle = CastShape<sf::RectangleShape>();
		rectangle->setPosition(Convert(screenPosition));
	}

	void RectangleRenderer::SetRotation(const Fixed screenRotation) {
		auto rectangle = CastShape<sf::RectangleShape>();
		rectangle->setRotation(screenRotation.GetFloat());
	}

	Vector2 RectangleRenderer::GetBounds() const {
		auto rectangle = CastShape<sf::RectangleShape>();
		return Vector2(
			static_cast<int>(rectangle->getSize().x),
			static_cast<int>(rectangle->getSize().y)
		);
	}
}
