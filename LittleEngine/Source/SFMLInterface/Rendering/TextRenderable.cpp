#include "le_stdafx.h"
#include "SFMLInterface/WindowController.h"
#include "RenderParams.h"
#include "TextRenderable.h"
#include "SFMLInterface/Assets.h"

namespace LittleEngine {
	TextData::TextData(FontAsset& font, const std::string & text) : TextData(font, text, 25, Colour::White, 0, Colour::Black) {
	}
	TextData::TextData(FontAsset& font, const std::string & text, Fixed pixelSize, Colour fillColour) : TextData(font, text, pixelSize, fillColour, 0, Colour::Black) {
	}
	TextData::TextData(FontAsset& font, const std::string & text, Fixed pixelSize, Colour fillColour, Fixed outlineSize, Colour outlineColour) : font(&font), text(text), pixelSize(pixelSize), fillColour(fillColour), outlineSize(outlineSize), outlineColour(outlineColour) {
	}

	void TextData::SetFont(FontAsset& font) {
		this->font = &font;
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

	TextRenderable::TextRenderable(const TextData & data) : Renderable("TextRenderable"), data(data) 
	{}

	TextRenderable::TextRenderable(const TextRenderable & prototype) : Renderable(prototype.name), data(prototype.data) {
	}

	std::unique_ptr<Renderable> TextRenderable::UClone() const {
		return std::make_unique<TextRenderable>(*this);
	}

	void TextRenderable::ApplyData() {
		text.setString(data.text);
		text.setFont(data.font->sfFont);
		text.setFillColor(Convert(data.fillColour));
		text.setOutlineColor(Convert(data.outlineColour));
		text.setOutlineThickness(data.outlineSize.Abs().ToFloat());
		text.setCharacterSize(data.pixelSize.Abs().ToInt());
		sf::FloatRect textRect = text.getLocalBounds();
		text.setOrigin(sf::Vector2f(
			textRect.width * data.GetNAlignmentHorz(),
			textRect.height * data.GetNAlignmentVert())
		);
	}

	void TextRenderable::SetPosition(const Vector2& screenPosition) {
		text.setPosition(Convert(screenPosition));
	}

	void TextRenderable::SetRotation(const Fixed& screenRotation) {
		text.setRotation(screenRotation.ToFloat());
	}

	void TextRenderable::SetScale(const Vector2 & screenScale) {
		text.setScale(Convert(screenScale));
	}

	void TextRenderable::RenderInternal(RenderParams & params) {
		ApplyData();
		SetPosition(params.screenPosition);
		SetRotation(params.screenRotation);
		SetScale(params.screenScale);
		params.GetWindowController().Push(Drawable(text, layer));
	}

	Rect2 TextRenderable::GetBounds() const {
		sf::FloatRect bounds = text.getLocalBounds();
		Fixed width(bounds.width);
		Fixed height(bounds.height);
		return Rect2(
			Vector2(-width * Fixed::OneHalf, -height * Fixed::OneHalf),
			Vector2(width * Fixed::OneHalf, height * Fixed::OneHalf)
		);
	}

	TextData& TextRenderable::GetTextData() {
		return data;
	}

	TextRenderable& TextRenderable::SetSize(const Fixed& pixelSize) {
		data.pixelSize = pixelSize;
		return *this;
	}

	TextRenderable& TextRenderable::SetColour(const Colour& colour) {
		data.fillColour = colour;
		return *this;
	}
}
