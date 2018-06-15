#pragma once
#include <memory>
#include "SFML/Graphics.hpp"
#include "Utils/Vector2.h"

namespace Game { 
	struct TextData;
	struct Colour;
	class CircleRenderer;
	class RectangleRenderer;
	class SpriteRenderer;
	class TextureAsset;
	class TextRenderer;
	

	namespace RenderFactory {
	std::unique_ptr<CircleRenderer> NewCircle(const Fixed& radius);
	std::unique_ptr<CircleRenderer> NewCircle(const Fixed& radius, const Colour& colour);
	std::unique_ptr<RectangleRenderer> NewRectangle(const Vector2& size);
	std::unique_ptr<RectangleRenderer> NewRectangle(const Vector2& size, const Colour& colour);
	std::unique_ptr<SpriteRenderer> NewSprite(const TextureAsset& texture);
	std::unique_ptr<TextRenderer> NewText(const TextData& data);
} }
