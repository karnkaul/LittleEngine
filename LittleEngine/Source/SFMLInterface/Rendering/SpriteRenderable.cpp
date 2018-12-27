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
		SetPivot(Vector2::Zero);
	}

	SpriteRenderable::SpriteRenderable(const SpriteRenderable & prototype, bool bSilent) : Renderable(prototype, bSilent), m_data(prototype.m_data) {
		ApplyData();
		SetPosition(Vector2::Zero);
		SetPivot(Vector2::Zero);
	}

	void SpriteRenderable::SetPosition(const Vector2& screenPosition) {
		m_sprite.setPosition(Graphics::Cast(screenPosition));
	}

	void SpriteRenderable::SetOrientation(const Fixed& screenOrientation) {
		m_sprite.setRotation(screenOrientation.ToFloat());
	}

	void SpriteRenderable::SetScale(const Vector2 & screenScale) {
		m_sprite.setScale(Graphics::Cast(screenScale));
	}

	std::unique_ptr<Renderable> SpriteRenderable::UClone() const {
		return std::make_unique<SpriteRenderable>(*this);
	}

	void SpriteRenderable::RenderInternal() {
		if (m_data.bDirty) ApplyData();
		Graphics::Submit(Drawable(m_sprite, m_layer));
	}

	void SpriteRenderable::SetPivot(const Vector2 & pivot) {
		Vector2 sfmlPivot((pivot.x + 1) * Fixed::OneHalf, (-pivot.y + 1) * Fixed::OneHalf);
		m_sprite.setOrigin(
			m_sprite.getLocalBounds().width * sfmlPivot.x.ToFloat(),
			m_sprite.getLocalBounds().height * sfmlPivot.y.ToFloat()
		);
	}

	void SpriteRenderable::Crop(const Rect2& rect) {
		sf::IntRect textureRect(rect.GetTopLeft().x.ToInt(), rect.GetTopLeft().y.ToInt(), rect.GetSize().x.ToInt(), rect.GetSize().y.ToInt());
		m_sprite.setTextureRect(textureRect);
	}

	Rect2 SpriteRenderable::GetBounds() const {
		sf::FloatRect bounds = m_sprite.getLocalBounds();
		Fixed width(bounds.width);
		Fixed height(bounds.height);
		return Rect2::CentreSize({ width, height });
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
		m_data.bDirty = false;
	}
}
