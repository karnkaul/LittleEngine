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
	TextData::TextData(FontAsset& font, const std::string & text, Fixed pixelSize, Colour fillColour, Fixed outlineSize, Colour outlineColour) : text(text), pixelSize(pixelSize), outlineSize(outlineSize), fillColour(fillColour),  outlineColour(outlineColour), pFont(&font) {
	}

	void TextData::SetFont(FontAsset& font) {
		this->pFont = &font;
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

	TextRenderable::TextRenderable(const TextData & data) : Renderable("TextRenderable"), m_data(data) {}
	TextRenderable::TextRenderable(const TextRenderable & prototype) : Renderable(prototype.m_name), m_data(prototype.m_data) {}

	std::unique_ptr<Renderable> TextRenderable::UClone() const {
		return std::make_unique<TextRenderable>(*this);
	}

	void TextRenderable::ApplyData() {
		m_sfText.setString(m_data.text);
		m_sfText.setFont(m_data.pFont->m_sfFont);
		m_sfText.setFillColor(Convert(m_data.fillColour));
		m_sfText.setOutlineColor(Convert(m_data.outlineColour));
		m_sfText.setOutlineThickness(m_data.outlineSize.Abs().ToFloat());
		m_sfText.setCharacterSize(m_data.pixelSize.Abs().ToInt());
		sf::FloatRect textRect = m_sfText.getLocalBounds();
		m_sfText.setOrigin(sf::Vector2f(
			textRect.width * m_data.GetNAlignmentHorz(),
			textRect.height * m_data.GetNAlignmentVert())
		);
	}

	void TextRenderable::SetPosition(const Vector2& screenPosition) {
		m_sfText.setPosition(Convert(screenPosition));
	}

	void TextRenderable::SetRotation(const Fixed& screenRotation) {
		m_sfText.setRotation(screenRotation.ToFloat());
	}

	void TextRenderable::SetScale(const Vector2 & screenScale) {
		m_sfText.setScale(Convert(screenScale));
	}

	void TextRenderable::RenderInternal(RenderParams & params) {
		ApplyData();
		SetPosition(params.screenPosition);
		SetRotation(params.screenRotation);
		SetScale(params.screenScale);
		params.GetWindowController().Push(Drawable(m_sfText, m_layer));
	}

	Rect2 TextRenderable::GetBounds() const {
		sf::FloatRect bounds = m_sfText.getLocalBounds();
		Fixed width(bounds.width);
		Fixed height(bounds.height);
		return Rect2(
			Vector2(-width * Fixed::OneHalf, -height * Fixed::OneHalf),
			Vector2(width * Fixed::OneHalf, height * Fixed::OneHalf)
		);
	}

	TextData& TextRenderable::GetTextData() {
		return m_data;
	}

	TextRenderable& TextRenderable::SetSize(const Fixed& pixelSize) {
		m_data.pixelSize = pixelSize;
		return *this;
	}

	TextRenderable& TextRenderable::SetColour(const Colour& colour) {
		m_data.fillColour = colour;
		return *this;
	}
}
