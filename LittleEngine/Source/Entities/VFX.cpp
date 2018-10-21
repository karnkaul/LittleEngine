#include "le_stdafx.h"
#include "VFX.h"
#include "Components/SpriteAnimator.h"
#include "SFMLInterface/Assets.h"
#include "SFMLInterface/Rendering/SpriteRenderer.h"
#include "Levels/Level.h"
#include "Engine/Audio/AudioManager.h"


namespace LittleEngine {
	VFX::VFX(Level & level, const std::string & prefix, const Vector2 & position, const Fixed & rotation) : Actor(level, prefix + "_VFX", position, rotation) {
	}
	
	void VFX::Init(const AssetPaths & spriteSheet, const AssetPaths& sfxPaths, const Fixed& animTime, const Fixed& sfxVol, bool autoplay) {
		if (!animator) {
			animator = AddComponent<SpriteAnimator>();
		}
		animator->SetSpriteSheet(spriteSheet, animTime);
		animator->SetLayer(LayerID::UI);
		animator->enabled = false;
		this->sfxPath = sfxPaths.GetRandom();
		this->sfxVol = sfxVol;
		if (autoplay) {
			Play();
		}
	}

	void VFX::Play() {
		sfxPlayer = level->GetAudioManager().PlaySFX(sfxPath, sfxVol);
		animator->enabled = true;
		animator->Start();
		playing = true;
	}

	void VFX::Tick(Fixed deltaTime) {
		Actor::Tick(deltaTime);
		if (playing) {
			if (!animator->IsAnimating() && !sfxPlayer->IsPlaying() && !_destroyed) {
				Destruct();
				playing = false;
			}
		}
	}
}
