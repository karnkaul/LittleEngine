#include "stdafx.h"
#include "SpriteAnimator.h"
#include "Levels/Level.h"
#include "Entities/Actor.h"
#include "Utils/Utils.h"
#include "SFMLInterface/Rendering/SpriteRenderer.h"
#include "SFMLInterface/Rendering/RenderFactory.h"

namespace LittleEngine {
	SpriteAnimator::SpriteAnimator(Actor& actor) : Component(actor, "AnimatedSprite") {	
	}

	void SpriteAnimator::SetSpriteSheet(const AssetPaths& spriteSheet, const Fixed & animTime, bool startAnim) {
		AssetManager& assetManager = GetActor().GetActiveLevel().GetAssetManager();
		std::vector<TextureAsset::Ptr> sheetAssets = assetManager.Load<TextureAsset>(spriteSheet.assetPaths);
		for (const auto& sprite : sheetAssets) {
			if (sprite) {
				sprites.push_back(sprite);
				if (!spriteRenderer) {
					SpriteData spriteData(sprite);
					spriteRenderer = RenderFactory::NewSprite(spriteData);
				}
			}
		}
		if (startAnim && animTime > Fixed::Zero) {
			this->animTime = animTime;
			Start();
		}
	}

	void SpriteAnimator::Start() {
		Reset();
		animating = true;
	}

	void SpriteAnimator::Stop() {
		animating = false;
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
		if (animating) {
			elapsed += deltaTime;
			Fixed ratio = Maths::Clamp01(elapsed / animTime);
			Fixed size = sprites.size();
			index = static_cast<size_t>((ratio * size).GetInt());
			if (index >= sprites.size()) {
				if (looping) {
					Reset();
				}
				else {
					Stop();
				}
			}
			else {
				SetSprite(sprites[index]);
			}
		}
	}

	void SpriteAnimator::Render(RenderParams params) {
		if (animating && spriteRenderer) {
			spriteRenderer->layer = LayerID::Player;
			spriteRenderer->Render(params);
		}
	}

	void SpriteAnimator::SetSprite(TextureAsset::Ptr sprite) {
		if (sprite) {
			if (!spriteRenderer) {
				SpriteData spriteData(sprite);
				spriteRenderer = RenderFactory::NewSprite(spriteData);
				spriteRenderer->layer = LayerID::Default;
			}
			spriteRenderer->SetEnabled(true);
			spriteRenderer->SetTexture(sprite);
		}
	}
}
