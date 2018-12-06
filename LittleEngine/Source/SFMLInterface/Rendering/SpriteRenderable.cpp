#include "le_stdafx.h"
#include "SpriteRenderable.h"
#include "ShapeRenderable.h"
#include "RenderParams.h"
#include "SFMLInterface/Graphics.h"
#include "../Graphics.h"

namespace LittleEngine {
	SpriteRenderable::SpriteRenderable(const SpriteData& data, bool bSilent) : Renderable("SpriteRenderable", bSilent), m_data(data) {
		ApplyData();
		SetPosition(Vector2::Zero);
	}

	SpriteRenderable::SpriteRenderable(const SpriteRenderable & prototype, bool bSilent) : Renderable(prototype, bSilent), m_data(prototype.m_data) {
		ApplyData();
		SetPosition(Vector2::Zero);
	}

	void SpriteRenderable::SetPosition(const Vector2& worldPosition) {
		m_sprite.setPosition(Graphics::Cast(worldPosition));
	}

	void SpriteRenderable::SetOrientation(const Fixed& worldOrientation) {
		m_sprite.setRotation(worldOrientation.ToFloat());
	}

	void SpriteRenderable::SetScale(const Vector2 & worldScale) {
		m_sprite.setScale(Graphics::Cast(worldScale));
	}

	std::unique_ptr<Renderable> SpriteRenderable::UClone() const {
		return std::make_unique<SpriteRenderable>(*this);
	}

	void SpriteRenderable::RenderInternal() {
		if (m_data.bDirty) ApplyData();
		Graphics::Submit(Drawable(m_sprite, m_layer));
	}

	Rect2 SpriteRenderable::GetBounds() const {
		sf::FloatRect bounds = m_sprite.getLocalBounds();
		Fixed width(bounds.width);
		Fixed height(bounds.height);
		return Rect2(
			Vector2(-width * Fixed::OneHalf, -height * Fixed::OneHalf),
			Vector2(width * Fixed::OneHalf, height * Fixed::OneHalf)
		);
	}

	void SpriteRenderable::SetTexture(TextureAsset& texture) {
		//Logger::Log(*this, "Setting texture to [" + texture->GetResourcePath() + "]", Logger::Severity::Debug);
		m_data.pTexture = &texture;
	}

	SpriteData& SpriteRenderable::GetData() {
		return m_data;
	}

	void SpriteRenderable::ApplyData() {
		m_sprite.setTexture(m_data.pTexture->m_sfTexture);
		m_sprite.setColor(Graphics::Cast(m_data.colour));
		m_sprite.setOrigin(
			m_sprite.getLocalBounds().width * 0.5f, 
			m_sprite.getLocalBounds().height * 0.5f
		);
		m_data.bDirty = false;
	}
}
