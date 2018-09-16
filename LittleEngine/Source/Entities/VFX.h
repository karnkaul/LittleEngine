#pragma once
#include "Actor.h"

namespace LittleEngine {
	struct AssetPaths;
	class SpriteAnimator;
	class SoundPlayer;

	class VFX : public Actor {
	public:
		VFX(Level& level, const std::string& prefix, const Vector2& position, const Fixed& rotation);

		void Init(const AssetPaths& spriteSheet, const AssetPaths& sfxPaths, const Fixed& animTime, const Fixed& sfxVol, bool autoplay = true);
		void Play();

		virtual void Tick(Fixed deltaTime) override;

	private:
		std::shared_ptr<SpriteAnimator> animator = nullptr;
		std::string sfxPath;
		Fixed sfxVol = Fixed(8, 10);
		SoundPlayer* sfxPlayer = nullptr;
		bool init = false;
		bool playing = false;
	};
}
