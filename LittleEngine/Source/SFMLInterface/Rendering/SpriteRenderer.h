#pragma once
#include "Vector2.h"
#include "Renderer.h"
#include "SFMLInterface/Assets.h"

namespace LittleEngine {
	// \brief Container for sprite rendering metadata
	struct SpriteData {
	public:
		Colour colour;

		SpriteData(TextureAsset& texture) : SpriteData(texture, Colour::White) {}
		SpriteData(TextureAsset& texture, const Colour& colour) : texture(&texture), colour(colour) {}

	private:
		friend class SpriteRenderer;
		TextureAsset* texture;
	};

	// \brief Conrete wrapper for SFML sprite 
	class SpriteRenderer : public Renderer {
	public:
		SpriteRenderer(const SpriteData& data);
		SpriteRenderer(const SpriteRenderer& prototype);

		void SetTexture(TextureAsset& texture);
		
		virtual Rect2 GetBounds() const override;

	protected:
		void ApplyData(); 

		virtual void RenderInternal(RenderParams& params) override;
		virtual void SetPosition(const Vector2& screenPosition) override;
		virtual void SetRotation(const Fixed& screenRotation) override;
		virtual std::unique_ptr<Renderer> UClone() const override;

	private:
		sf::Sprite sprite;
		SpriteData data;
	};
}
