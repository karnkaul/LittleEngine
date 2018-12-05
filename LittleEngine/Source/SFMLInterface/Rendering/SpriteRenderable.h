#pragma once
#include "Vector2.h"
#include "Renderable.h"
#include "SFMLInterface/Assets.h"

namespace LittleEngine {
	// \brief Container for sprite rendering metadata
	struct SpriteData {
	public:
		Colour colour;
	private:
		TextureAsset* pTexture;
		
	public:
		SpriteData(TextureAsset& texture) : SpriteData(texture, Colour::White) {}
		SpriteData(TextureAsset& texture, const Colour& colour) : colour(colour), pTexture(&texture) {}

	private:
		friend class SpriteRenderable;
	};

	// \brief Conrete wrapper for SFML sprite 
	class SpriteRenderable : public Renderable {
	private:
		sf::Sprite m_sprite;
		SpriteData m_data;

	public:
		SpriteRenderable(const SpriteData& data, bool bSilent = false);
		SpriteRenderable(const SpriteRenderable& prototype, bool bSilent = false);

		void SetTexture(TextureAsset& texture);
		SpriteData& GetData();
		
		virtual Rect2 GetBounds() const override;

	protected:
		void ApplyData(); 

		virtual void RenderInternal(RenderParams& params) override;
		virtual void SetPosition(const Vector2& screenPosition) override;
		virtual void SetRotation(const Fixed& screenRotation) override;
		virtual void SetScale(const Vector2& screenScale) override;
		virtual std::unique_ptr<Renderable> UClone() const override;
	};
}
