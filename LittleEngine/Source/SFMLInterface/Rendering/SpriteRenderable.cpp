#include "le_stdafx.h"
#include "SpriteRenderable.h"
#include "ShapeRenderable.h"
#include "RenderParams.h"
#include "SFMLInterface/WindowController.h"

namespace LittleEngine {
	SpriteRenderable::SpriteRenderable(const SpriteData& data, bool bSilent) : Renderable("SpriteRenderable", bSilent), data(data) {
		ApplyData();
		SetPosition(Vector2::Zero);
	}

	SpriteRenderable::SpriteRenderable(const SpriteRenderable & prototype, bool bSilent) : Renderable(prototype, bSilent), data(prototype.data) {
		ApplyData();
		SetPosition(Vector2::Zero);
	}

	void SpriteRenderable::SetPosition(const Vector2& screenPosition) {
		sprite.setPosition(Convert(screenPosition));
	}

	void SpriteRenderable::SetRotation(const Fixed& screenRotation) {
		sprite.setRotation(screenRotation.ToFloat());
	}

	void SpriteRenderable::SetScale(const Vector2 & screenScale) {
		sprite.setScale(Convert(screenScale));
	}

	std::unique_ptr<Renderable> SpriteRenderable::UClone() const {
		return std::make_unique<SpriteRenderable>(*this);
	}

	void SpriteRenderable::RenderInternal(RenderParams & params) {
		ApplyData();
		SetPosition(params.screenPosition);
		SetRotation(params.screenRotation);
		SetScale(params.screenScale);
		params.GetWindowController().Push(Drawable(sprite, layer));
	}

	Rect2 SpriteRenderable::GetBounds() const {
		sf::FloatRect bounds = sprite.getLocalBounds();
		Fixed width(bounds.width);
		Fixed height(bounds.height);
		return Rect2(
			Vector2(-width * Fixed::OneHalf, -height * Fixed::OneHalf),
			Vector2(width * Fixed::OneHalf, height * Fixed::OneHalf)
		);
	}

	void SpriteRenderable::SetTexture(TextureAsset& texture) {
		//Logger::Log(*this, "Setting texture to [" + texture->GetResourcePath() + "]", Logger::Severity::Debug);
		data.texture = &texture;
	}

	SpriteData& SpriteRenderable::GetData() {
		return data;
	}

	void SpriteRenderable::ApplyData() {
		sprite.setTexture(data.texture->sfTexture);
		sprite.setColor(Convert(data.colour));
		sprite.setOrigin(
			sprite.getLocalBounds().width * 0.5f, 
			sprite.getLocalBounds().height * 0.5f
		);
	}
}
