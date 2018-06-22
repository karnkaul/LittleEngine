#pragma once
#include "Utils/Vector2.h"
#include "Renderer.h"
#include "SFMLInterface/Assets.h"

namespace Game {
	class SpriteRenderer : public Renderer {
	public:
		SpriteRenderer(TextureAsset::Ptr texture);
		virtual void SetPosition(const Vector2 screenPosition) override;
		virtual void SetRotation(const Fixed screenRotation) override;
		virtual void Render(RenderParams& params) override;
		virtual Vector2 GetBounds() const override;
		void SetTexture(TextureAsset::Ptr texture);
	protected:
		TextureAsset::Ptr texture;
		sf::Sprite sprite;
	};
}
