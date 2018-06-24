#pragma once
#include "Utils/Vector2.h"
#include "Renderer.h"
#include "SFMLInterface/Assets.h"

namespace Game {
	struct SpriteData {
	public:
		SpriteData(TextureAsset::Ptr texture) : texture(texture) {}
		Colour colour;
	private:
		friend class SpriteRenderer;
		TextureAsset::Ptr texture;
	};

	class SpriteRenderer : public Renderer {
	public:
		SpriteRenderer(const SpriteData& data);
		virtual void SetPosition(const Vector2 screenPosition) override;
		virtual void SetRotation(const Fixed screenRotation) override;
		virtual void Render(RenderParams& params) override;
		virtual Vector2 GetBounds() const override;
		void SetTexture(TextureAsset::Ptr texture);
	protected:
		void ApplyData();
	private:
		SpriteData data;
		sf::Sprite sprite;
	};
}
