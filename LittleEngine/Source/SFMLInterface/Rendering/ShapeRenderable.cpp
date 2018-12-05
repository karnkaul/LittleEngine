#include "le_stdafx.h"
#include "ShapeRenderable.h"
#include "Engine/Logger/Logger.h"
#include "SFMLInterface/WindowController.h"
#include "RenderParams.h"

namespace LittleEngine {
	ShapeRenderable::ShapeRenderable(std::string name, std::unique_ptr<sf::Shape> shape) : Renderable(name), m_uShape(std::move(shape)) {
	}

	ShapeRenderable::ShapeRenderable(const ShapeRenderable & prototype, std::unique_ptr<sf::Shape> shape) : Renderable(prototype), m_uShape(std::move(shape)) {
	}

	void ShapeRenderable::RenderInternal(RenderParams & params) {
		SetPosition(params.screenPosition);
		SetRotation(params.screenRotation);
		SetScale(params.screenScale);
		// Push shape into appropriate drawing layer
		params.GetWindowController().Push(Drawable(*m_uShape, m_layer));
	}

	void ShapeRenderable::SetFillColour(const Colour & colour) {
		m_uShape->setFillColor(Convert(colour));
	}

	void ShapeRenderable::SetBorder(const Fixed& width, const Colour & colour) {
		m_uShape->setOutlineColor(Convert(colour));
		m_uShape->setOutlineThickness(width > 0 ? width.ToFloat() : 0);
	}

	void ShapeRenderable::SetPosition(const Vector2& screenPosition) {
		m_uShape->setPosition(Convert(screenPosition));
	}

	void ShapeRenderable::SetRotation(const Fixed& screenRotation) {
		m_uShape->setRotation(screenRotation.ToFloat());
	}

	void ShapeRenderable::SetScale(const Vector2 & screenScale) {
		m_uShape->setScale(Convert(screenScale));
	}

	CircleRenderable::CircleRenderable(const Fixed& radius) : ShapeRenderable("CircleRenderable", std::make_unique<sf::CircleShape>(radius.ToFloat())) {
		m_pCircle = &CastShape<sf::CircleShape>();
		m_pCircle->setOrigin(radius.ToFloat(), radius.ToFloat());
	}

	CircleRenderable::CircleRenderable(const Fixed& radius, const Colour& colour) : ShapeRenderable("CircleRenderable", std::make_unique<sf::CircleShape>(radius.ToFloat())) {
		m_pCircle = &CastShape<sf::CircleShape>();
		m_pCircle->setOrigin(radius.ToFloat(), radius.ToFloat());
		SetFillColour(colour);
	}

	CircleRenderable::CircleRenderable(const CircleRenderable & prototype) : ShapeRenderable(prototype, std::make_unique<sf::CircleShape>()) {
		m_pCircle = &CastShape<sf::CircleShape>();
		m_pCircle->setOrigin(prototype.m_pCircle->getOrigin());
		m_pCircle->setRadius(prototype.m_pCircle->getRadius());
		m_pCircle->setFillColor(prototype.m_pCircle->getFillColor());
		m_pCircle->setOutlineColor(prototype.m_pCircle->getOutlineColor());
		m_pCircle->setOutlineThickness(prototype.m_pCircle->getOutlineThickness());
	}

	void CircleRenderable::SetRadius(const Fixed& radius) {
		m_pCircle->setRadius(radius.ToFloat());
		// Reposition local centre
		m_pCircle->setOrigin(radius.ToFloat(), radius.ToFloat());
	}

	Rect2 CircleRenderable::GetBounds() const {
		Fixed radius(m_pCircle->getRadius());
		return Rect2(
			Vector2(-radius, -radius),
			Vector2(radius, radius)
		);
	}

	std::unique_ptr<Renderable> CircleRenderable::UClone() const {
		return std::make_unique<CircleRenderable>(*this);
	}

	RectangleRenderable::RectangleRenderable(const Vector2& size) : ShapeRenderable("RectangleRenderable", std::make_unique<sf::RectangleShape>(sf::Vector2f(size.x.ToFloat(), size.y.ToFloat()))) {
		m_pRectangle = &CastShape<sf::RectangleShape>();
		m_pRectangle->setOrigin(m_pRectangle->getSize().x * 0.5f, m_pRectangle->getSize().y * 0.5f);
	}

	RectangleRenderable::RectangleRenderable(const Vector2& size, const Colour& colour) : ShapeRenderable("RectangleRenderable", std::make_unique<sf::RectangleShape>(sf::Vector2f(size.x.ToFloat(), size.y.ToFloat()))) {
		m_pRectangle = &CastShape<sf::RectangleShape>();
		m_pRectangle->setOrigin(m_pRectangle->getSize().x * 0.5f, m_pRectangle->getSize().y * 0.5f);
		SetFillColour(colour);
	}

	RectangleRenderable::RectangleRenderable(const RectangleRenderable & prototype) : ShapeRenderable(prototype, std::make_unique<sf::RectangleShape>()) {
		m_pRectangle = &CastShape<sf::RectangleShape>();
		m_pRectangle->setOrigin(prototype.m_pRectangle->getOrigin());
		m_pRectangle->setSize(prototype.m_pRectangle->getSize());
		m_pRectangle->setFillColor(prototype.m_pRectangle->getFillColor());
		m_pRectangle->setOutlineColor(prototype.m_pRectangle->getOutlineColor());
		m_pRectangle->setOutlineThickness(prototype.m_pRectangle->getOutlineThickness());
	}

	void RectangleRenderable::SetSize(const Vector2& size) {
		m_pRectangle->setSize(Convert(size));
		// Reposition local centre
		m_pRectangle->setOrigin(m_pRectangle->getSize().x * 0.5f, m_pRectangle->getSize().y * 0.5f);
	}

	Rect2 RectangleRenderable::GetBounds() const {
		sf::Vector2f size = m_pRectangle->getSize();
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
