#pragma once
#include "Component.h"
#include "Utils/Vector2.h"
#include "SFMLInterface/Rendering/Renderer.h"

namespace Game {
	class Actor;
	struct RenderParams;
	class Renderer;
	class CircleRenderer;
	class RectangleRenderer;
	class TextRenderer;
	class SpriteRenderer;

	struct ShapeData {
		Vector2 size;
		Colour colour;
		ShapeData(const Vector2& size) : ShapeData(size, Colour::White) {}
		ShapeData(const Vector2& size, const Colour& colour) : size(size), colour(colour) {}
	};

	class RenderComponent : public Component {
	public:
		RenderComponent(Actor& actor);
		void SetRenderer(std::unique_ptr<Renderer> renderer);
		Vector2 GetBounds() const;
		Vector2 GetWorldBounds(const class World& world) const;
		CircleRenderer& SetCircleRenderer(const ShapeData& shapeData);
		RectangleRenderer& SetRectangleRenderer(const ShapeData& shapeData);
		SpriteRenderer& SetSpriteRenderer(const std::string& texturePath);
		TextRenderer& SetTextRenderer(const std::string& text);
		virtual void Render(RenderParams& params) override;
	private:
		std::unique_ptr<Renderer> renderer = nullptr;
	};
}
