#include "le_stdafx.h"
#include "SpriteRenderable.h"
#include "ShapeRenderable.h"
#include "RenderParams.h"
#include "SFMLInterface/WindowController.h"

namespace LittleEngine {
	SpriteRenderable::SpriteRenderable(const SpriteData& data, bool bSilent) : Renderable("SpriteRenderable", bSilent), m_data(data) {
		ApplyData();
		SetPosition(Vector2::Zero);
	}

	SpriteRenderable::SpriteRenderable(const SpriteRenderable & prototype, bool bSilent) : Renderable(prototype, bSilent), m_data(prototype.m_data) {
		ApplyData();
		SetPosition(Vector2::Zero);
	}

	void SpriteRenderable::SetPosition(const Vector2& screenPosition) {
		m_sprite.setPosition(Convert(screenPosition));
	}

	void SpriteRenderable::SetRotation(const Fixed& screenRotation) {
		m_sprite.setRotation(screenRotation.ToFloat());
	}

	void SpriteRenderable::SetScale(const Vector2 & screenScale) {
		m_sprite.setScale(Convert(screenScale));
	}

	std::unique_ptr<Renderable> SpriteRenderable::UClone() const {
		return std::make_unique<SpriteRenderable>(*this);
	}

	void SpriteRenderable::RenderInternal(RenderParams & params) {
		ApplyData();
		SetPosition(params.screenPosition);
		SetRotation(params.screenRotation);
		SetScale(params.screenScale);
		params.GetWindowController().Push(Drawable(m_sprite, m_layer));
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
		m_sprite.setColor(Convert(m_data.colour));
		m_sprite.setOrigin(
			m_sprite.getLocalBounds().width * 0.5f, 
			m_sprite.getLocalBounds().height * 0.5f
		);
	}
}
