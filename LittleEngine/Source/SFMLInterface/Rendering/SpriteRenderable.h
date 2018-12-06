#pragma once
#include "Vector2.h"
#include "Renderable.h"
#include "SFMLInterface/Assets.h"

namespace LittleEngine {
	// \brief Container for sprite rendering metadata
	struct SpriteData {
	private:
		bool bDirty = false;
		Colour colour;
		TextureAsset* pTexture;
		
	public:
		SpriteData(TextureAsset& texture) : SpriteData(texture, Colour::White) {}
		SpriteData(TextureAsset& texture, const Colour& colour) : colour(colour), pTexture(&texture) {}
		
		void SetColour(Colour colour) {
			this->colour = colour;
			bDirty = true;
		}

		Colour GetColour() const { return colour; }

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

		virtual void RenderInternal() override;
		virtual void SetPosition(const Vector2& worldPosition) override;
		virtual void SetOrientation(const Fixed& worldOrientation) override;
		virtual void SetScale(const Vector2& worldScale) override;
		virtual std::unique_ptr<Renderable> UClone() const override;
	};
}
