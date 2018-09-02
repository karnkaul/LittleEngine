#pragma once
#include "Component.h"
#include "Utils/Rect2.h"
#include "SFMLInterface/Rendering/Renderer.h"

namespace Game {
	class Actor;
	struct RenderParams;
	class Renderer;
	class CircleRenderer;
	class RectangleRenderer;
	class TextRenderer;
	class SpriteRenderer;

	// \brief Structure loosely describing a shape to be constructed
	struct ShapeData {
		Vector2 size;
		Colour colour;

		ShapeData(const Vector2& size) : ShapeData(size, Colour::White) {}
		ShapeData(const Vector2& size, const Colour& colour) : size(size), colour(colour) {}
	};

	class RenderComponent : public Component {
	public:
		RenderComponent(Actor& actor);

		LayerInfo GetLayer() const;
		void SetLayer(const LayerInfo& layer);
		Rect2 GetBounds() const;

		CircleRenderer& SetCircleRenderer(const ShapeData& shapeData);
		RectangleRenderer& SetRectangleRenderer(const ShapeData& shapeData);
		SpriteRenderer& SetSpriteRenderer(const std::string& texturePath);
		TextRenderer& SetTextRenderer(const std::string& text);

		virtual void Render(RenderParams params) override;
		
	protected:
		void SetRenderer(std::unique_ptr<Renderer> renderer);

	private:
		std::unique_ptr<Renderer> renderer = nullptr;
	};
}
