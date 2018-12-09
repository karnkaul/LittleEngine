#include "le_stdafx.h"
#include "UIElement.h"
#include "Levels/Level.h"
#include "SFMLInterface/Assets.h"
#include "SFMLInterface/Rendering/Renderable.h"
#include "SFMLInterface/Rendering/SpriteRenderable.h"
#include "SFMLInterface/Rendering/TextRenderable.h"
#include "SFMLInterface/Rendering/ShapeRenderable.h"

namespace LittleEngine {
	UIElement::UIElement() : Object("UIElement") {
		Logger::Log(*this, "Untitled UIElement constructed", Logger::Severity::Debug);
	}

	UIElement::UIElement(const std::string & name) : Object(name) {
		Logger::Log(*this, GetNameInBrackets() + " constructed", Logger::Severity::Debug);
	}

	UIElement::~UIElement() {
		Logger::Log(*this, GetNameInBrackets() + " destroyed", Logger::Severity::Debug);
	}

	void UIElement::SetLevel(Level & level) {
		this->pLevel = &level;
	}

	void UIElement::SetPanel(UByte r, UByte g, UByte b, UByte a) {
		Colour colour(r, g, b, a);
		SetPanel(colour);
	}

	void UIElement::SetPanel(Colour colour) {
		if (!m_uPanel) {
			m_uPanel = std::make_unique<RectangleRenderable>(m_transform.size, colour);
		}
		else {
			m_uPanel->SetFillColour(colour);
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

	void UIElement::SetText(const std::string & text) {
		if (!m_uText) {
			FontAsset* font = pLevel->GetAssetManager().GetDefaultFont();
			TextData data(*font, text);
			m_uText = std::make_unique<TextRenderable>(data);
		}
		else {
			m_uText->GetTextData().SetText(text);
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

	void UIElement::Tick(Fixed deltaTime) {
		m_transform.anchor.x = Maths::Clamp_11(m_transform.anchor.x);
		m_transform.anchor.y = Maths::Clamp_11(m_transform.anchor.y);
		if (m_uText) {
			m_uText->SetPivot(m_transform.anchor);
		}
		if (m_uSprite) {
			m_uSprite->SetPivot(m_transform.anchor);
		}
		if (m_uPanel) {
			m_uPanel->SetSize(m_transform.size);
			m_uPanel->SetPivot(m_transform.anchor);
		}
	}

	void UIElement::Render() {
		if (m_uText || m_uSprite || m_uPanel) {
			RenderParams params(m_transform.GetWorldPosition(), Fixed::Zero, Vector2::One);
			if (m_uPanel) m_uPanel->Render(params);
			if (m_uSprite) m_uSprite->Render(params);
			if (m_uText) m_uText->Render(params);
		}
	}
}
