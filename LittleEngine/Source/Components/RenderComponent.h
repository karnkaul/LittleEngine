#pragma once
#include "Component.h"
#include "Rect2.h"
#include "SFMLInterface/Rendering/Renderer.h"
#include "SFMLInterface/Assets.h"

namespace LittleEngine {
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
		RenderComponent(Actor& owner, const RenderComponent& prototype);

		LayerInfo GetLayer() const;
		void SetLayer(const LayerInfo& layer);
		Rect2 GetBounds() const;

		CircleRenderer& SetCircleRenderer(const ShapeData& shapeData);
		RectangleRenderer& SetRectangleRenderer(const ShapeData& shapeData);
		SpriteRenderer& SetSpriteRenderer(const std::string& texturePath);
		SpriteRenderer& SetSpriteRenderer(TextureAsset& texture);
		TextRenderer& SetTextRenderer(const std::string& text);

		virtual void Render(RenderParams params) override;
		virtual Component::Ptr SClone(Actor& owner) const override;
		
	protected:
		void SetRenderer(std::unique_ptr<Renderer> renderer);

	private:
		std::unique_ptr<Renderer> renderer = nullptr;
	};
}
