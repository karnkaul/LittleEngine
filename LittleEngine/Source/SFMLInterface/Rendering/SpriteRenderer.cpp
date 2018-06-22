#include "stdafx.h"
#include "SpriteRenderer.h"
#include "ShapeRenderer.h"
#include "RenderParams.h"
#include "SFMLInterface/WindowController.h"

namespace Game {
	SpriteRenderer::SpriteRenderer(TextureAsset::Ptr texture) : Renderer("SpriteRenderer"), texture(texture) {
		SetTexture(texture);
		SetPosition(Vector2::Zero);
	}

	void SpriteRenderer::SetPosition(const Vector2 screenPosition) {
		sprite.setPosition(Convert(screenPosition));
	}

	void SpriteRenderer::SetRotation(const Fixed screenRotation) {
		sprite.setRotation(screenRotation.GetFloat());
	}

	void SpriteRenderer::Render(RenderParams & params) {
		SetPosition(params.screenPosition);
		SetRotation(params.screenRotation);
		params.GetWindowController().Draw(sprite);
	}

	Vector2 SpriteRenderer::GetBounds() const {
		return Vector2(
			static_cast<int>(sprite.getLocalBounds().width),
			static_cast<int>(sprite.getLocalBounds().height)
		);
	}

	void SpriteRenderer::SetTexture(TextureAsset::Ptr texture) {
		Logger::Log(*this, "Setting texture to [" + texture->GetResourcePath() + "]", Logger::Severity::Debug);
		sprite.setTexture(texture->sfTexture);
		sprite.setOrigin(sprite.getLocalBounds().width * 0.5f, sprite.getLocalBounds().height * 0.5f);
	}
}
