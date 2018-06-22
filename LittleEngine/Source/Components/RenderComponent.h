#pragma once
#include "Component.h"
#include "Utils/Vector2.h"

namespace Game {
	class Actor;
	struct RenderParams;
	class Renderer;
	class CircleRenderer;
	class RectangleRenderer;
	class TextRenderer;
	class SpriteRenderer;

	class RenderComponent : public Component {
	public:
		RenderComponent(Actor& actor);
		void SetRenderer(std::unique_ptr<Renderer> renderer);
		Vector2 GetBounds() const;
		Vector2 GetWorldBounds(const class World& world) const;
		CircleRenderer& SetCircleRenderer(std::unique_ptr<CircleRenderer> circleRenderer);
		RectangleRenderer& SetRectangleRenderer(std::unique_ptr<RectangleRenderer> rectangleRenderer);
		SpriteRenderer& SetSpriteRenderer(const std::string& texturePath);
		TextRenderer& SetTextRenderer(const std::string& text);
		virtual void Render(RenderParams& params) override;
	private:
		std::unique_ptr<Renderer> renderer = nullptr;
	};
}
