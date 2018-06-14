#pragma once
#include <string>
#include "Renderer.h"

namespace Game {
	class FontAsset;
	class TextureAsset;

	enum Style {

	};

	struct TextData {
	public:
		Fixed pixelSize;
		std::string text;
		Colour fillColour;
		Fixed outlineSize;
		Colour outlineColour;

		TextData(FontAsset& font, const std::string& text);
		TextData(FontAsset& font, const std::string& text, Fixed pixelSize, Colour fillColour);
		TextData(FontAsset& font, const std::string& text, Fixed pixelSize, Colour fillColour, Fixed outlineSize, Colour outlineColour);
		void SetFont(FontAsset& font);

	private:
		friend class TextRenderer;
		FontAsset* font;
	};

	class TextRenderer : public Renderer {
	public:
		TextRenderer(const TextData& data);
		virtual void SetPosition(const Vector2 screenPosition) override;
		virtual void SetRotation(const Fixed screenRotation) override;
		virtual void Render(struct RenderParams& params) override;
		virtual Vector2 GetBounds() const override;
		TextData& GetTextData();
		TextRenderer& SetSize(Fixed pixelSize);
		TextRenderer& SetColour(Colour colour);
	protected:
		void ApplyData();
	private:
		TextData data;
		sf::Text text;
	};
}
