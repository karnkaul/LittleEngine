#include "le_stdafx.h"
#include "SpriteAnimator.h"
#include "Levels/Level.h"
#include "Entities/Actor.h"
#include "Utils.h"
#include "SFMLInterface/Rendering/SpriteRenderer.h"

namespace LittleEngine {
	SpriteAnimator::SpriteAnimator(Actor& actor) : Component(actor, "AnimatedSprite") {	
	}

	SpriteAnimator::SpriteAnimator(Actor& owner, const SpriteAnimator & prototype) : Component(owner, prototype), sprites(prototype.sprites), bAnimating(prototype.bAnimating), animTime(prototype.animTime) {
		if (!sprites.empty() && sprites[0]) {
			spriteRenderer = std::make_unique<SpriteRenderer>(SpriteData(*sprites[0]));
		}
		else {
			Logger::Log(*this, "SpriteAnimator cloned with empty spritesheet!", Logger::Severity::Error);
		}
	}

	void SpriteAnimator::SetSpriteSheet(const AssetPaths& spriteSheet, const Fixed & animTime, bool bStartAnim) {
		AssetManager& assetManager = GetActor().GetActiveLevel().GetAssetManager();
		std::vector<TextureAsset*> sheetAssets = assetManager.Load<TextureAsset>(spriteSheet.assetPaths);
		for (const auto& sprite : sheetAssets) {
			if (sprite) {
				sprites.push_back(sprite);
				if (!spriteRenderer) {
					SpriteData spriteData(*sprite);
					spriteRenderer = std::make_unique<SpriteRenderer>(spriteData);
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
		if (spriteRenderer) {
			spriteRenderer->SetEnabled(false);
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
		if (spriteRenderer) {
			spriteRenderer->layer = layer;
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
			index = static_cast<size_t>((ratio * size).GetInt());
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
		if (bAnimating && spriteRenderer) {
			spriteRenderer->layer = LayerID::Player;
			spriteRenderer->Render(params);
		}
	}

	std::shared_ptr<Component> SpriteAnimator::SClone(Actor& owner) const {
		return std::make_shared<SpriteAnimator>(owner, *this);
	}

	void SpriteAnimator::SetSprite(TextureAsset& sprite) {
		if (!spriteRenderer) {
			SpriteData spriteData(sprite);
			spriteRenderer = std::make_unique<SpriteRenderer>(spriteData);
			spriteRenderer->layer = LayerID::Default;
		}
		spriteRenderer->SetEnabled(true);
		spriteRenderer->SetTexture(sprite);
	}
}
