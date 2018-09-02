#include "stdafx.h"
#include "SpriteRenderer.h"
#include "ShapeRenderer.h"
#include "RenderParams.h"
#include "SFMLInterface/WindowController.h"

namespace Game {
	SpriteRenderer::SpriteRenderer(const SpriteData& data) : Renderer("SpriteRenderer"), data(data) {
		ApplyData();
		SetPosition(Vector2::Zero);
	}

	void SpriteRenderer::SetPosition(const Vector2& screenPosition) {
		sprite.setPosition(Convert(screenPosition));
	}

	void SpriteRenderer::SetRotation(const Fixed& screenRotation) {
		sprite.setRotation(screenRotation.GetFloat());
	}

	void SpriteRenderer::RenderInternal(RenderParams & params) {
		ApplyData();
		SetPosition(params.screenPosition);
		SetRotation(params.screenRotation);
		params.GetWindowController().Push(Drawable(sprite, layer));
	}

	Rect2 SpriteRenderer::GetBounds() const {
		sf::FloatRect bounds = sprite.getLocalBounds();
		Fixed width(bounds.width);
		Fixed height(bounds.height);
		return Rect2(
			Vector2(-width * Fixed::Half, -height * Fixed::Half),
			Vector2(width * Fixed::Half, height * Fixed::Half)
		);
	}

	void SpriteRenderer::SetTexture(TextureAsset::Ptr texture) {
		Logger::Log(*this, "Setting texture to [" + texture->GetResourcePath() + "]", Logger::Severity::Debug);
		data.texture = texture;
	}

	void SpriteRenderer::ApplyData() {
		sprite.setTexture(data.texture->sfTexture);
		sprite.setColor(Convert(data.colour));
		sprite.setOrigin(
			sprite.getLocalBounds().width * 0.5f, 
			sprite.getLocalBounds().height * 0.5f
		);
	}
}
