#pragma once
#include "Component.h"
#include "RenderComponent.h"
#include "SFMLInterface/Assets.h"

namespace LittleEngine {
	// \brief Swaps textures on a SpriteRenderable over a set period of time
	class SpriteAnimator final : public Component {
	private:
		std::unique_ptr<SpriteRenderable> m_uSprite;
		std::vector<TextureAsset*> m_sprites;
		size_t m_index = 0;
		Fixed m_elapsed = Fixed::Zero;
		Fixed m_animTime = Fixed::Zero;
		bool m_bAnimating = false;
	public:
		bool m_bLooping = false;

		SpriteAnimator(Actor& actor);
		SpriteAnimator(Actor& owner, const SpriteAnimator& prototype);

		void SetSprite(TextureAsset& texture);
		void SetSpriteSheet(const AssetPaths& spriteSheet, const Fixed& animTime, bool bStartAnim = true);
		void Start();
		void Stop();
		void Reset();
		bool IsAnimating() const { return m_bAnimating; }
		void SetLayer(LayerInfo layer);

		virtual void Tick(const Fixed& deltaMS) override;
		virtual void Render(const RenderParams& params) override;
		virtual Component::Ptr UClone(Actor& owner) const override;
	};
}
