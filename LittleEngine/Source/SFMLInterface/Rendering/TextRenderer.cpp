#include "stdafx.h"
#include "SFMLInterface/WindowController.h"
#include "RenderParams.h"
#include "TextRenderer.h"
#include "SFMLInterface/Assets.h"

namespace Game {
	TextData::TextData(FontAsset & font, const std::string & text)
		: TextData(font, text, 25, Colour::White, 0, Colour::Black) {
	}
	TextData::TextData(FontAsset & font, const std::string & text, Fixed pixelSize, Colour fillColour)
		: TextData(font, text, pixelSize, fillColour, 0, Colour::Black) {
	}
	TextData::TextData(FontAsset & font, const std::string & text, Fixed pixelSize, Colour fillColour, Fixed outlineSize, Colour outlineColour)
		: font(&font), text(text), pixelSize(pixelSize), fillColour(fillColour), outlineSize(outlineSize), outlineColour(outlineColour) {
	}
	void TextData::SetFont(FontAsset & font) {
		this->font = &font;
	}

	TextRenderer::TextRenderer(const TextData & data) :
		Renderer("TextRenderer"), data(data) {
	}

	void TextRenderer::ApplyData() {
		text.setString(data.text);
		text.setFont(data.font->sfFont);
		text.setFillColor(Convert(data.fillColour));
		text.setOutlineColor(Convert(data.outlineColour));
		text.setOutlineThickness(data.outlineSize.Abs().GetFloat());
		text.setCharacterSize(data.pixelSize.Abs().GetInt());
		text.setOrigin(sf::Vector2f(
			text.getLocalBounds().width * 0.5f,
			text.getLocalBounds().height * 0.5f)
		);
	}

	void TextRenderer::SetPosition(const Vector2 screenPosition) {
		text.setPosition(Convert(screenPosition));
	}

	void TextRenderer::SetRotation(const Fixed screenRotation) {
		text.setRotation(screenRotation.GetFloat());
	}

	void TextRenderer::Render(RenderParams & params) {
		ApplyData();
		SetPosition(params.screenPosition);
		SetRotation(params.screenRotation);
		params.GetWindowController().Draw(text);
	}

	Vector2 TextRenderer::GetBounds() const {
		return Vector2(
			static_cast<int>(text.getLocalBounds().width),
			static_cast<int>(text.getLocalBounds().height)
		);
	}

	TextData& TextRenderer::GetTextData() {
		return data;
	}

	TextRenderer& TextRenderer::SetSize(Fixed pixelSize) {
		data.pixelSize = pixelSize;
		return *this;
	}

	TextRenderer& TextRenderer::SetColour(Colour colour) {
		data.fillColour = colour;
		return *this;
	}
}
