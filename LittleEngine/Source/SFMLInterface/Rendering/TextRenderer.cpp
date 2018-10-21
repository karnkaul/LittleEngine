#include "le_stdafx.h"
#include "SFMLInterface/WindowController.h"
#include "RenderParams.h"
#include "TextRenderer.h"
#include "SFMLInterface/Assets.h"

namespace LittleEngine {
	TextData::TextData(FontAsset::Ptr font, const std::string & text)
		: TextData(font, text, 25, Colour::White, 0, Colour::Black) {
	}
	TextData::TextData(FontAsset::Ptr font, const std::string & text, Fixed pixelSize, Colour fillColour)
		: TextData(font, text, pixelSize, fillColour, 0, Colour::Black) {
	}
	TextData::TextData(FontAsset::Ptr font, const std::string & text, Fixed pixelSize, Colour fillColour, Fixed outlineSize, Colour outlineColour)
		: font(font), text(text), pixelSize(pixelSize), fillColour(fillColour), outlineSize(outlineSize), outlineColour(outlineColour) {
	}

	void TextData::SetFont(FontAsset::Ptr font) {
		this->font = font;
	}

	float TextData::GetNAlignmentHorz() const {
		switch (hAlign) {
		default:
		case HAlign::Centre:
			return 0.5f;
		case HAlign::Left:
			return 0.0f;
		case HAlign::Right:
			return 1.0f;
		}
	}

	float TextData::GetNAlignmentVert() const {
		switch (vAlign) {
		default:
		case VAlign::Centre:
			return 0.5f;
		case VAlign::Bottom:
			return 0.0f;
		case VAlign::Top:
			return 1.0f;
		}
	}

	TextRenderer::TextRenderer(const TextData & data) : Renderer("TextRenderer"), data(data) 
	{}

	void TextRenderer::ApplyData() {
		text.setString(data.text);
		text.setFont(data.font->sfFont);
		text.setFillColor(Convert(data.fillColour));
		text.setOutlineColor(Convert(data.outlineColour));
		text.setOutlineThickness(data.outlineSize.Abs().GetFloat());
		text.setCharacterSize(data.pixelSize.Abs().GetInt());
		sf::FloatRect textRect = text.getLocalBounds();
		text.setOrigin(sf::Vector2f(
			textRect.width * data.GetNAlignmentHorz(),
			textRect.height * data.GetNAlignmentVert())
		);
	}

	void TextRenderer::SetPosition(const Vector2& screenPosition) {
		text.setPosition(Convert(screenPosition));
	}

	void TextRenderer::SetRotation(const Fixed& screenRotation) {
		text.setRotation(screenRotation.GetFloat());
	}

	void TextRenderer::RenderInternal(RenderParams & params) {
		ApplyData();
		SetPosition(params.screenPosition);
		SetRotation(params.screenRotation);
		params.GetWindowController().Push(Drawable(text, layer));
	}

	Rect2 TextRenderer::GetBounds() const {
		sf::FloatRect bounds = text.getLocalBounds();
		Fixed width(bounds.width);
		Fixed height(bounds.height);
		return Rect2(
			Vector2(-width * Fixed::OneHalf, -height * Fixed::OneHalf),
			Vector2(width * Fixed::OneHalf, height * Fixed::OneHalf)
		);
	}

	TextData& TextRenderer::GetTextData() {
		return data;
	}

	TextRenderer& TextRenderer::SetSize(const Fixed& pixelSize) {
		data.pixelSize = pixelSize;
		return *this;
	}

	TextRenderer& TextRenderer::SetColour(const Colour& colour) {
		data.fillColour = colour;
		return *this;
	}
}
