#pragma once
#include <memory>
#include "UIObject.h"
#include "UITransform.h"
#include "SFMLInterface/Rendering/Renderable.h"
#include "SFMLInterface/Rendering/TextRenderable.h"

namespace LittleEngine {
	struct UIText {
		Fixed pixelSize;
		std::string text;
		Colour colour;

		UIText();
		UIText(const char* text);
		UIText(const std::string& text);
		UIText(std::string&& text);
		UIText(const std::string& text, const Fixed& pixelSize, Colour colour);
	};

	// \brief Base UI WorldEntity class: uses UITransform, is capable of drawing a panel/an image/some text
	class UIElement : public UIObject {
	public:
		UITransform m_transform;
		LayerID m_layer = LayerID::UI;
	protected:
		std::unique_ptr<class SpriteRenderable> m_uSprite;
		std::unique_ptr<class TextRenderable> m_uText;
		std::unique_ptr<class RectangleRenderable> m_uPanel;
	private:
		class Level* pLevel;
		bool bSilent;

	public:
		UIElement(bool bSilent = false);
		UIElement(const std::string& name, bool bSilent = false);
		virtual ~UIElement();

		void SetPanel(UByte r = 255, UByte g = 255, UByte b = 255, UByte a = 128);
		void SetPanel(Colour fill, const Fixed& border = Fixed::Zero, Colour outline = Colour::Transparent);
		void SetImage(class TextureAsset& texture, Colour colour = Colour::White);
		void SetText(const UIText& uiText);
		void SetFont(class FontAsset& font);
		TextData* GetTextData() const;

		virtual void Tick(const Fixed& deltaMS) override;
		virtual void Render() override;

	public:
		void InitElement(Level& level, UITransform* pParent = nullptr);

	private:
		friend class UIController;
		friend class UIWidget;
		friend class UIContext;
	};
}
