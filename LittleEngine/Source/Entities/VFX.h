#pragma once
#include "Actor.h"
#include "SFMLInterface/Assets.h"

namespace LittleEngine {
	struct AssetPaths;
	class SpriteAnimator;
	class SoundPlayer;

	class VFX : public Actor {
	public:
		VFX(Level& level, const std::string& prefix, const Vector2& position, const Fixed& rotation);
		VFX(Level& owner, const VFX& prototytpe);

		void Init(const AssetPaths& spriteSheet, const AssetPaths& sfxPaths, const Fixed& animTime, const Fixed& sfxVol, bool autoplay = true);
		void Play();

		virtual void Tick(const Fixed& deltaTime) override;

	private:
		SpriteAnimator* animator = nullptr;
		std::vector<SoundAsset*> soundAssets;
		Fixed sfxVol = Fixed(8, 10);
		SoundPlayer* sfxPlayer = nullptr;
		bool bPlaying = false;
	};
}
