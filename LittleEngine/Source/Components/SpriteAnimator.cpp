#include "le_stdafx.h"
#include "SpriteAnimator.h"
#include "Levels/Level.h"
#include "Entities/Actor.h"
#include "Utils.h"
#include "SFMLInterface/Rendering/SpriteRenderable.h"

namespace LittleEngine {
	SpriteAnimator::SpriteAnimator(Actor& actor) : Component(actor, "AnimatedSprite") {	
	}

	SpriteAnimator::SpriteAnimator(Actor& owner, const SpriteAnimator & prototype) : Component(owner, prototype), sprites(prototype.sprites), animTime(prototype.animTime), bAnimating(prototype.bAnimating) {
		if (!sprites.empty() && sprites[0]) {
			sprite = std::make_unique<SpriteRenderable>(SpriteData(*sprites[0]));
		}
		else {
			Logger::Log(*this, "SpriteAnimator cloned with empty spritesheet!", Logger::Severity::Error);
		}
	}

	void SpriteAnimator::SetSpriteSheet(const AssetPaths& spriteSheet, const Fixed & animTime, bool bStartAnim) {
		AssetManager& assetManager = GetActor().GetActiveLevel().GetAssetManager();
		std::vector<TextureAsset*> sheetAssets = assetManager.Load<TextureAsset>(spriteSheet.assetPaths);
		for (const auto& spriteAsset : sheetAssets) {
			if (spriteAsset) {
				sprites.push_back(spriteAsset);
				if (!spriteAsset) {
					SpriteData spriteData(*spriteAsset);
					sprite = std::make_unique<SpriteRenderable>(spriteData);
				}
			}
		}
		if (bStartAnim && animTime > Fixed::Zero) {
			this->animTime = animTime;
			Start();
		}
	}

	void SpriteAnimator::Start() {
		Reset();
		bAnimating = true;
	}

	void SpriteAnimator::Stop() {
		bAnimating = false;
		if (sprite) {
			sprite->SetEnabled(false);
		}
		else {
			Logger::Log(*this, "Stop() called before spritesheet set!", Logger::Severity::Warning);
		}
	}

	void SpriteAnimator::Reset() {
		index = 0;
		elapsed = Fixed::Zero;
	}

	void SpriteAnimator::SetLayer(LayerInfo layer) {
		if (sprite) {
			sprite->layer = layer;
		}
		else {
			Logger::Log(*this, "SetLayer() called before spritesheet set!", Logger::Severity::Warning);
		}
	}

	void SpriteAnimator::Tick(Fixed deltaTime) {
		if (bAnimating) {
			elapsed += deltaTime;
			Fixed ratio = Maths::Clamp01(elapsed / animTime);
			Fixed size = static_cast<int>(sprites.size());
			index = static_cast<size_t>((ratio * size).ToInt());
			if (index >= sprites.size()) {
				if (bLooping) {
					Reset();
				}
				else {
					Stop();
				}
			}
			else {
				SetSprite(*sprites[index]);
			}
		}
	}

	void SpriteAnimator::Render(RenderParams params) {
		if (bAnimating && sprite) {
			sprite->layer = LayerID::Player;
			sprite->Render(params);
		}
	}

	Component::Ptr SpriteAnimator::UClone(Actor& owner) const {
		return std::make_unique<SpriteAnimator>(owner, *this);
	}

	void SpriteAnimator::SetSprite(TextureAsset& texture) {
		if (!sprite) {
			SpriteData spriteData(texture);
			sprite = std::make_unique<SpriteRenderable>(spriteData);
			sprite->layer = LayerID::Default;
		}
		sprite->SetEnabled(true);
		sprite->SetTexture(texture);
	}
}
