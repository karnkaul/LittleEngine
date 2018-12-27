#include "le_stdafx.h"
#include "SFMLInterface/Graphics.h"
#include "RenderParams.h"
#include "TextRenderable.h"
#include "SFMLInterface/Assets.h"
#include "../Graphics.h"

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

	void TextData::SetText(const std::string & text) {
		this->text = text;
		bDirty = true;
	}

	void TextData::SetPixelSize(Fixed pixelSize) {
		this->pixelSize = pixelSize;
		bDirty = true;
	}

	void TextData::SetFillColour(Colour fillColour) {
		this->fillColour = fillColour;
		bDirty = true;
	}

	void TextData::SetOutlineColour(Colour outlineColour) {
		this->outlineColour = outlineColour;
		bDirty = true;
	}

	void TextData::SetHAlign(HAlign hAlign) {
		this->hAlign = hAlign;
		bDirty = true;
	}

	void TextData::SetVAlign(VAlign vAlign) {
		this->vAlign = vAlign;
		bDirty = true;
	}

	const std::string & TextData::GetText() const {
		return text;
	}

	Fixed TextData::GetPixelSize() const {
		return pixelSize;
	}

	Colour TextData::GetFillColour() const {
		return fillColour;
	}

	Colour TextData::GetOutlineColour() const {
		return outlineColour;
	}

	HAlign TextData::GetHAlign() const {
		return hAlign;
	}

	VAlign TextData::GetVAlign() const {
		return vAlign;
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

	TextRenderable::TextRenderable(const TextData & data, bool bSilent) : Renderable("TextRenderable", bSilent), m_data(data) {
		ApplyData();
	}

	TextRenderable::TextRenderable(const TextRenderable & prototype) : Renderable(prototype.m_name), m_data(prototype.m_data) {
		ApplyData();
	}

	std::unique_ptr<Renderable> TextRenderable::UClone() const {
		return std::make_unique<TextRenderable>(*this);
	}

	void TextRenderable::ApplyData() {
		m_sfText.setString(m_data.text);
		m_sfText.setFont(m_data.pFont->m_sfFont);
		m_sfText.setFillColor(Graphics::Cast(m_data.fillColour));
		m_sfText.setOutlineColor(Graphics::Cast(m_data.outlineColour));
		m_sfText.setOutlineThickness(m_data.outlineSize.Abs().ToFloat());
		m_sfText.setCharacterSize(m_data.pixelSize.Abs().ToInt());
		sf::FloatRect textRect = m_sfText.getLocalBounds();
		if (!m_data.bCustomPivot) {
			m_sfText.setOrigin(sf::Vector2f(
				textRect.width * m_data.GetNAlignmentHorz(),
				textRect.height * m_data.GetNAlignmentVert())
			);
		}
		m_data.bDirty = false;
	}

	void TextRenderable::SetPosition(const Vector2& screenPosition) {
		m_sfText.setPosition(Graphics::Cast(screenPosition));
	}

	void TextRenderable::SetOrientation(const Fixed& screenOrientation) {
		m_sfText.setRotation(screenOrientation.ToFloat());
	}

	void TextRenderable::SetScale(const Vector2 & screenScale) {
		m_sfText.setScale(Graphics::Cast(screenScale));
	}

	void TextRenderable::RenderInternal() {
		if (m_data.bDirty) ApplyData();
		Graphics::Submit(Drawable(m_sfText, m_layer));
	}

	void TextRenderable::SetPivot(const Vector2 & pivot) {
		Vector2 sfmlPivot((pivot.x + 1) * Fixed::OneHalf, (-pivot.y + 1) * Fixed::OneHalf);
		m_sfText.setOrigin(
			m_sfText.getLocalBounds().width * sfmlPivot.x.ToFloat(),
			m_sfText.getLocalBounds().height * sfmlPivot.y.ToFloat()
		);
		m_data.bCustomPivot = true;
	}

	Rect2 TextRenderable::GetBounds() const {
		sf::FloatRect bounds = m_sfText.getLocalBounds();
		Fixed width(bounds.width);
		Fixed height(bounds.height);
		return Rect2::CentreSize({ width, height });
	}

	TextData& TextRenderable::GetTextData() {
		return m_data;
	}

	TextRenderable& TextRenderable::SetSize(const Fixed& pixelSize) {
		m_data.SetPixelSize(pixelSize);
		return *this;
	}

	TextRenderable& TextRenderable::SetColour(const Colour& colour) {
		m_data.SetFillColour(colour);
		return *this;
	}
}
