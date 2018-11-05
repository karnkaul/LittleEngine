#include "le_stdafx.h"
#include "VFX.h"
#include "Components/SpriteAnimator.h"
#include "SFMLInterface/Rendering/SpriteRenderer.h"
#include "Levels/Level.h"
#include "Engine/Audio/AudioManager.h"
#include "Utils.h"

namespace LittleEngine {
	VFX::VFX(Level & level, const std::string & prefix, const Vector2 & position, const Fixed & rotation) : Actor(level, prefix + "_VFX", position, rotation) {
	}

	VFX::VFX(Level& owner, const VFX & prototype) : Actor(owner, prototype), soundAssets(prototype.soundAssets), sfxVol(prototype.sfxVol) {
		std::shared_ptr<Component> animatorComponent = prototype.animator->SClone(*this);
		animator = dynamic_cast<SpriteAnimator*>(animatorComponent.get());
		components.push_back(animatorComponent);
		animator->Stop();
		animator->Reset();
	}
	
	void VFX::Init(const AssetPaths & spriteSheet, const AssetPaths& sfxPaths, const Fixed& animTime, const Fixed& sfxVol, bool autoplay) {
		if (!animator) {
			animator = AddComponent<SpriteAnimator>();
		}
		animator->SetSpriteSheet(spriteSheet, animTime);
		animator->SetLayer(LayerID::UI);
		animator->bEnabled = false;
		soundAssets = level->GetAssetManager().Load<SoundAsset>(sfxPaths.assetPaths);
		this->sfxVol = sfxVol;
		if (autoplay) {
			Play();
		}
	}

	void VFX::Play() {
		ToggleActive(true);
		Fixed direction = GetTransform().Position().x;
		if (!soundAssets.empty()) {
			size_t randIdx = Maths::Random::Range((size_t)0, soundAssets.size());
			SoundAsset* random = soundAssets[randIdx];
			if (random) sfxPlayer = level->GetAudioManager().PlaySFX(*random, sfxVol, direction);
		}
		animator->bEnabled = true;
		animator->Start();
		bPlaying = true;
	}

	void VFX::Tick(Fixed deltaTime) {
		Actor::Tick(deltaTime);
		if (bPlaying) {
			bool sfxPlaying = sfxPlayer && sfxPlayer->IsPlaying();
			if (!animator->IsAnimating() && !sfxPlaying && !_bDestroyed) {
				Destruct();
				bPlaying = false;
			}
		}
	}
}
