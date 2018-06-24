#include "stdafx.h"
#include "RenderFactory.h"
#include "SFML/Graphics.hpp"
#include "ShapeRenderer.h"
#include "SpriteRenderer.h"
#include "TextRenderer.h"

namespace Game { namespace RenderFactory {
	std::unique_ptr<CircleRenderer> NewCircle(const Fixed& radius) {
		return std::make_unique<Game::CircleRenderer>(radius);
	}
	std::unique_ptr<CircleRenderer> NewCircle(const Fixed& radius, const Colour& colour) {
		return std::make_unique<Game::CircleRenderer>(radius, colour);
	}
	std::unique_ptr<RectangleRenderer> NewRectangle(const Vector2& size) {
		return std::make_unique<Game::RectangleRenderer>(size);
	}
	std::unique_ptr<RectangleRenderer> NewRectangle(const Vector2& size, const Colour& colour) {
		return std::make_unique<Game::RectangleRenderer>(size, colour);
	}
	std::unique_ptr<SpriteRenderer> NewSprite(const SpriteData& data) {
		return std::make_unique<SpriteRenderer>(data);
	}
	std::unique_ptr<TextRenderer> NewText(const TextData& data) {
		return std::make_unique<TextRenderer>(data);
	}
} }
