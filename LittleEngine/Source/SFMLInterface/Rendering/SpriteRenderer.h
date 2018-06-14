#pragma once
#include "Utils/Vector2.h"
#include "Renderer.h"

namespace Game {
	class TextureAsset;

	class SpriteRenderer : public Renderer {
	public:
		SpriteRenderer(const TextureAsset& texture);
		virtual void SetPosition(const Vector2 screenPosition) override;
		virtual void SetRotation(const Fixed screenRotation) override;
		virtual void Render(RenderParams& params) override;
		virtual Vector2 GetBounds() const override;
		void SetTexture(const TextureAsset& texture);
	protected:
		const TextureAsset* texture;
		sf::Sprite sprite;
	};
}
