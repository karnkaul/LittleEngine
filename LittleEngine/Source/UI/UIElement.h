#pragma once
#include <memory>
#include "Engine/Object.h"
#include "UITransform.h"
#include "SFMLInterface/Rendering/Renderable.h"

namespace LittleEngine {
	class UIElement : public Object {
	public:
		UITransform m_transform;
	protected:
		std::unique_ptr<class SpriteRenderable> m_uSprite;
		std::unique_ptr<class TextRenderable> m_uText;
		std::unique_ptr<class RectangleRenderable> m_uPanel;
	private:
		class Level* pLevel;

	public:
		UIElement();
		UIElement(const std::string& name);
		virtual ~UIElement();

		void SetPanel(UByte r = 255, UByte g = 255, UByte b = 255, UByte a = 128);
		void SetPanel(Colour colour);
		void SetImage(class TextureAsset& texture, Colour colour = Colour::White);
		void SetText(const std::string& text);
		void SetFont(class FontAsset& font);

		virtual void Tick(Fixed deltaTime);
		virtual void Render();

	protected:

		//TODO: Make Private
	public:
		void SetLevel(Level& level);

		friend class Level;
	};
}
