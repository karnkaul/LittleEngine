#pragma once
#include "Component.h"
#include "RenderComponent.h"
#include "SFMLInterface/Assets.h"

namespace LittleEngine {
	// \brief Swaps textures on a SpriteRenderable over a set period of time
	class SpriteAnimator final : public Component {
	public:
		SpriteAnimator(Actor& actor);
		SpriteAnimator(Actor& owner, const SpriteAnimator& prototype);

		void SetSpriteSheet(const AssetPaths& spriteSheet, const Fixed& animTime, bool bStartAnim = true);
		void Start();
		void Stop();
		void Reset();
		bool IsAnimating() const { return bAnimating; }
		void SetLayer(LayerInfo layer);

		virtual void Tick(Fixed deltaTime) override;
		virtual void Render(RenderParams& params) override;
		virtual Component::Ptr UClone(Actor& owner) const override;

	private:
		std::unique_ptr<SpriteRenderable> sprite = nullptr;
		std::vector<TextureAsset*> sprites;
		size_t index = 0;
		Fixed elapsed = Fixed::Zero;
		Fixed animTime = Fixed::Zero;
		bool bAnimating = false;

	public:
		bool bLooping = false;

		void SetSprite(TextureAsset& texture);
	};
}
