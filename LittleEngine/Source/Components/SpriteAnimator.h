#pragma once
#include "Component.h"
#include "RenderComponent.h"
#include "SFMLInterface/Assets.h"

namespace LittleEngine {
	// \brief Swaps textures on a SpriteRenderer over a set period of time
	class SpriteAnimator final : public Component {
	public:
		bool looping = false;

		SpriteAnimator(Actor& actor);
		void SetSpriteSheet(const AssetPaths& spriteSheet, const Fixed& animTime, bool startAnim = true);
		void Start();
		void Stop();
		void Reset();
		bool IsAnimating() const { return animating; }
		void SetLayer(LayerInfo layer);

		virtual void Tick(Fixed deltaTime) override;
		virtual void Render(RenderParams params) override;

	private:
		std::unique_ptr<SpriteRenderer> spriteRenderer = nullptr;
		std::vector<TextureAsset::Ptr> sprites;
		size_t index = 0;
		Fixed elapsed = Fixed::Zero;
		Fixed animTime = Fixed::Zero;
		bool animating = false;

		void SetSprite(TextureAsset::Ptr sprite);
	};
}
