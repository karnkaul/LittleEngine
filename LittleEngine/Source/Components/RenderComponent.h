#pragma once
#include "Component.h"
#include "Rect2.h"
#include "SFMLInterface/Rendering/Renderable.h"
#include "SFMLInterface/Assets.h"

namespace LittleEngine {
	class Actor;
	struct RenderParams;
	class Renderable;
	class CircleRenderable;
	class RectangleRenderable;
	class TextRenderable;
	class SpriteRenderable;

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

		CircleRenderable& SetCircleRenderable(const ShapeData& shapeData);
		RectangleRenderable& SetRectangleRenderable(const ShapeData& shapeData);
		SpriteRenderable& SetSpriteRenderable(const std::string& texturePath);
		SpriteRenderable& SetSpriteRenderable(TextureAsset& texture);
		TextRenderable& SetTextRenderable(const std::string& text);

		virtual void Render(RenderParams params) override;
		virtual Component::Ptr UClone(Actor& owner) const override;
		
	protected:
		void SetRenderable(std::unique_ptr<Renderable> renderable);

	private:
		std::unique_ptr<Renderable> uRenderable = nullptr;
	};
}
