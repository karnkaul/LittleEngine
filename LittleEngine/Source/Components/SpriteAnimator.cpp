#include "le_stdafx.h"
#include "SpriteAnimator.h"
#include "Levels/Level.h"
#include "Entities/Actor.h"
#include "Utils.h"
#include "SFMLInterface/Rendering/SpriteRenderable.h"

namespace LittleEngine {
	SpriteAnimator::SpriteAnimator(Actor& actor) : Component(actor, "AnimatedSprite") {	
	}

	SpriteAnimator::SpriteAnimator(Actor& owner, const SpriteAnimator & prototype) : Component(owner, prototype), m_sprites(prototype.m_sprites), m_animTime(prototype.m_animTime), m_bAnimating(prototype.m_bAnimating) {
		if (!m_sprites.empty() && m_sprites[0]) {
			m_uSprite = std::make_unique<SpriteRenderable>(SpriteData(*m_sprites[0]));
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
				m_sprites.push_back(spriteAsset);
				if (!spriteAsset) {
					SpriteData spriteData(*spriteAsset);
					m_uSprite = std::make_unique<SpriteRenderable>(spriteData);
				}
			}
		}
		if (bStartAnim && animTime > Fixed::Zero) {
			this->m_animTime = animTime;
			Start();
		}
	}

	void SpriteAnimator::Start() {
		Reset();
		m_bAnimating = true;
	}

	void SpriteAnimator::Stop() {
		m_bAnimating = false;
		if (m_uSprite) {
			m_uSprite->SetEnabled(false);
		}
		else {
			Logger::Log(*this, "Stop() called before spritesheet set!", Logger::Severity::Warning);
		}
	}

	void SpriteAnimator::Reset() {
		m_index = 0;
		m_elapsed = Fixed::Zero;
	}

	void SpriteAnimator::SetLayer(LayerInfo layer) {
		if (m_uSprite) {
			m_uSprite->m_layer = layer;
		}
		else {
			Logger::Log(*this, "SetLayer() called before spritesheet set!", Logger::Severity::Warning);
		}
	}

	void SpriteAnimator::Tick(Fixed deltaTime) {
		if (m_bAnimating) {
			m_elapsed += deltaTime;
			Fixed ratio = Maths::Clamp01(m_elapsed / m_animTime);
			Fixed size = static_cast<int>(m_sprites.size());
			m_index = static_cast<size_t>((ratio * size).ToInt());
			if (m_index >= m_sprites.size()) {
				if (m_bLooping) {
					Reset();
				}
				else {
					Stop();
				}
			}
			else {
				SetSprite(*m_sprites[m_index]);
			}
		}
	}

	void SpriteAnimator::Render(const RenderParams& params) {
		if (m_bAnimating && m_uSprite) {
			m_uSprite->m_layer = LayerID::Player;
			m_uSprite->Render(params);
		}
	}

	Component::Ptr SpriteAnimator::UClone(Actor& owner) const {
		return std::make_unique<SpriteAnimator>(owner, *this);
	}

	void SpriteAnimator::SetSprite(TextureAsset& texture) {
		if (!m_uSprite) {
			SpriteData spriteData(texture);
			m_uSprite = std::make_unique<SpriteRenderable>(spriteData);
			m_uSprite->m_layer = LayerID::Default;
		}
		m_uSprite->SetEnabled(true);
		m_uSprite->SetTexture(texture);
	}
}
