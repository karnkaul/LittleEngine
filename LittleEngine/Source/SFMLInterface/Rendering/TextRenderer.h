#pragma once
#include <string>
#include "Renderer.h"
#include "SFMLInterface/Assets.h"

namespace Game {
	enum Style {

	};

	struct TextData {
	public:
		Fixed pixelSize;
		std::string text;
		Colour fillColour;
		Fixed outlineSize;
		Colour outlineColour;

		TextData(FontAsset::Ptr font, const std::string& text);
		TextData(FontAsset::Ptr font, const std::string& text, Fixed pixelSize, Colour fillColour);
		TextData(FontAsset::Ptr font, const std::string& text, Fixed pixelSize, Colour fillColour, Fixed outlineSize, Colour outlineColour);
		void SetFont(FontAsset::Ptr font);

	private:
		friend class TextRenderer;
		FontAsset::Ptr font;
	};

	class TextRenderer : public Renderer {
	public:
		TextRenderer(const TextData& data);
		virtual void Render(struct RenderParams& params) override;
		virtual Rect2 GetBounds() const override;
		TextData& GetTextData();
		TextRenderer& SetSize(Fixed pixelSize);
		TextRenderer& SetColour(Colour colour);
	protected:
		virtual void SetPosition(const Vector2 screenPosition) override;
		virtual void SetRotation(const Fixed screenRotation) override;
		void ApplyData();
	private:
		TextData data;
		sf::Text text;
	};
}
