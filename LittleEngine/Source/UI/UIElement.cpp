#include "le_stdafx.h"
#include "UIElement.h"
#include "Levels/Level.h"
#include "SFMLInterface/Assets.h"
#include "SFMLInterface/Rendering/Renderable.h"
#include "SFMLInterface/Rendering/SpriteRenderable.h"
#include "SFMLInterface/Rendering/ShapeRenderable.h"

namespace LittleEngine {
	UIText::UIText() : pixelSize(20), colour(Colour::Black) {}
	UIText::UIText(const char * text) : pixelSize(20), text(std::string(text)), colour(Colour::Black) {}
	UIText::UIText(const std::string & text) : pixelSize(20), text(text), colour(Colour::Black) {}
	UIText::UIText(std::string && text) : pixelSize(20), text(std::move(text)), colour(Colour::Black) {}
	UIText::UIText(const std::string & text, const Fixed & pixelSize, Colour colour) : pixelSize(pixelSize), text(text), colour(colour) {}

	UIElement::UIElement() : UIObject("UIElement") {
		Logger::Log(*this, "Untitled UIElement constructed", Logger::Severity::Debug);
	}

	UIElement::UIElement(const std::string & name) : UIObject(name) {
		Logger::Log(*this, GetNameInBrackets() + " constructed", Logger::Severity::Debug);
	}

	UIElement::~UIElement() {
		Logger::Log(*this, GetNameInBrackets() + " destroyed", Logger::Severity::Debug);
	}

	void UIElement::InitElement(Level & level, UITransform* pParent) {
		this->pLevel = &level;
		if (pParent) m_transform.SetParent(*pParent);
	}

	void UIElement::SetPanel(UByte r, UByte g, UByte b, UByte a) {
		Colour colour(r, g, b, a);
		SetPanel(colour);
	}

	void UIElement::SetPanel(Colour fill, const Fixed& border, Colour outline) {
		if (!m_uPanel) {
			m_uPanel = std::make_unique<RectangleRenderable>(m_transform.size, fill);
			m_uPanel->SetBorder(border, outline);
		}
		else {
			m_uPanel->SetFillColour(fill);
			m_uPanel->SetBorder(border, outline);
		}
	}

	void UIElement::SetImage(TextureAsset & texture, Colour colour) {
		if (!m_uSprite) {
			SpriteData data(texture, colour);
			m_uSprite = std::make_unique<SpriteRenderable>(data);
		}
		else {
			m_uSprite->SetTexture(texture);
			m_uSprite->GetData().SetColour(colour);
		}
	}

	void UIElement::SetText(const UIText & uiText) {
		if (!m_uText) {
			FontAsset* font = pLevel->GetAssetManager().GetDefaultFont();
			TextData data(*font, uiText.text, uiText.pixelSize, uiText.colour);
			m_uText = std::make_unique<TextRenderable>(data);
		}
		else {
			TextData& data = m_uText->GetTextData();
			data.SetText(uiText.text);
			data.SetPixelSize(uiText.pixelSize);
			data.SetFillColour(uiText.colour);
		}
	}

	void UIElement::SetFont(FontAsset & font) {
		if (!m_uText) {
			TextData data(font, "[Text]");
			m_uText = std::make_unique<TextRenderable>(data);
		}
		else {
			m_uText->GetTextData().SetFont(font);
		}
	}

	TextData* UIElement::GetTextData() const {
		if (m_uText) return &m_uText->GetTextData();
		return nullptr;
	}

	void UIElement::Tick(const Fixed&) {
		if (m_bDestroyed) return;
		m_transform.anchor.x = Maths::Clamp_11(m_transform.anchor.x);
		m_transform.anchor.y = Maths::Clamp_11(m_transform.anchor.y);
		if (m_uText) {
			m_uText->SetPivot(m_transform.anchor);
			m_uText->m_layer = m_layer;
		}
		if (m_uSprite) {
			m_uSprite->SetPivot(m_transform.anchor);
			m_uSprite->m_layer = m_layer;
		}
		if (m_uPanel) {
			m_uPanel->SetSize(m_transform.size);
			m_uPanel->SetPivot(m_transform.anchor);
			m_uPanel->m_layer = m_layer;
		}
	}

	void UIElement::Render() {
		if (m_bDestroyed) return;
		if (m_uText || m_uSprite || m_uPanel) {
			RenderParams params(m_transform.GetWorldPosition(), Fixed::Zero, Vector2::One);
			if (m_uPanel) m_uPanel->Render(params);
			if (m_uSprite) m_uSprite->Render(params);
			if (m_uText) m_uText->Render(params);
		}
	}
}
