#pragma once
#include <memory>
#include "Utils/Vector2.h"
#include "SFML/Graphics.hpp"
#include "ShapeRenderer.h"
#include "SpriteRenderer.h"
#include "TextRenderer.h"
#include "SFMLInterface/Assets.h"

namespace Game {
	/*static*/ class RenderFactory {
	public:
		static std::unique_ptr<CircleRenderer> NewCircle(const Fixed& radius) {
			return std::make_unique<Game::CircleRenderer>(radius);
		}
		static std::unique_ptr<CircleRenderer> NewCircle(const Fixed& radius, const Colour& colour) {
			return std::make_unique<Game::CircleRenderer>(radius, colour);
		}
		static std::unique_ptr<RectangleRenderer> NewRectangle(const Vector2& size) {
			return std::make_unique<Game::RectangleRenderer>(size);
		}
		static std::unique_ptr<RectangleRenderer> NewRectangle(const Vector2& size, const Colour& colour) {
			return std::make_unique<Game::RectangleRenderer>(size, colour);
		}
		static std::unique_ptr<SpriteRenderer> NewSprite(const TextureAsset& texture) {
			return std::make_unique<SpriteRenderer>(texture);
		}
		static std::unique_ptr<TextRenderer> NewText(const TextData& data) {
			return std::make_unique<TextRenderer>(data);
		}
	};
}
